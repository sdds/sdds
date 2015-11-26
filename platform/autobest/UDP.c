#include <os-ssal/Memory.h>
#include <os-ssal/Thread.h>
#include <stdio.h>
#include <string.h>

#include <sdds/DataSink.h>
#include <sdds/Log.h>
#include <sdds/LocatorDB.h>
#include <sdds/NetBuffRef.h>
#include <sdds/sdds_types.h>
#include <gen_constants.h>

#include <api_calls.h>
#include <lwip/opt.h>
#include <lwip/api.h>
#include <lwip/inet.h>
#include <lwip/mld6.h>
#include <lwip/netif.h>
#include <lwip/err.h>

#ifdef UTILS_DEBUG
#define PLATFORM_AUTOBEST_IPV6_MAX_CHAR_LEN 46
#endif

#ifndef PLATFORM_AUTOBEST_SDDS_PORT
#define PLATFORM_AUTOBEST_SDDS_PORT 23234
#endif

#ifndef PLATFORM_AUTOBEST_SDDS_PROTOCOL
// better use AF_INET6
#define PLATFORM_AUTOBEST_SDDS_PROTOCOL AF_INET6
#endif

#define PLATFORM_AUTOBEST_SDDS_BUILTIN_MULTICAST_ADDRESS                SDDS_BUILTIN_MULTICAST_ADDRESS
#define PLATFORM_AUTOBEST_SDDS_BUILTIN_MULTICAST_PARTICIPANT_ADDRESS    SDDS_BUILTIN_PARTICIPANT_ADDRESS
#define PLATFORM_AUTOBEST_SDDS_BUILTIN_MULTICAST_TOPIC_ADDRESS          SDDS_BUILTIN_TOPIC_ADDRESS
#define PLATFORM_AUTOBEST_SDDS_BUILTIN_MULTICAST_SUB_PUB_ADDRESS        SDDS_BUILTIN_SUB_PUB_ADDRESS

#define PLATFORM_AUTOBEST_SDDS_BUILTIN_MULTICAST_PORT_OFF 20

#ifndef PLATFORM_AUTOBEST_SDDS_ADDRESS
#define PLATFORM_AUTOBEST_SDDS_ADDRESS "::1"
#endif


struct Network_t {
    struct netconn* fd_uni_conn;
    struct netconn* fd_multi_conn;
    int port;
    Thread recvThread;
    Thread multiRecvThread;
};

//typedef struct Network_t Network;

struct AutobestLocator_str
{
  struct Locator_t loc;
  ip_addr_t addr_storage;
  int port;
};

typedef struct AutobestLocator_str AutobestLocator_t;

static struct Network_t net;
static NetBuffRef_t inBuff;
static NetBuffRef_t multiInBuff;

void* recvLoop(void*);
static struct netconn* create_connection(int port);


// for the builtintopic
// IF BUILTIN
Locator builtinTopicNetAddress;
// ENDIF

rc_t Network_Multicast_init(void);
rc_t Network_createLocator(Locator* loc);
rc_t Network_createMulticastLocator(Locator* loc);
rc_t Network_Multicast_joinMulticastGroup(char *group);



size_t Network_size(void)
{
  return sizeof(struct Network_t);
}

rc_t Network_init(void)
{

  // search in the db for the locator
  // TODO do something better than this hack here....
  net.fd_uni_conn = create_connection(PLATFORM_AUTOBEST_SDDS_PORT);
  if( net.fd_uni_conn == NULL){
    Log_error("error create_connection\n");
    return SDDS_RT_FAIL;
  }
  // init the incoming frame buffer and add dummy unicast locator
  NetBuffRef_init(&inBuff);
  ssw_rc_t ret = Network_createLocator(&inBuff.addr);
  if(ret == SDDS_RT_FAIL)
  {
    Log_error("error while init incoming Buffer\n");
    return SDDS_RT_FAIL;
  }

  // set up a thread to read from the udp socket
  net.recvThread = Thread_create();
  if(net.recvThread == NULL){
    sys_task_terminate();
    sys_abort();
  }
  ret = Thread_init(net.recvThread, recvLoop, (void *)&inBuff);
  if(ret != SDDS_RT_OK){
    sys_task_terminate();
    sys_abort();
  }


#ifdef _MULTICAST
  ret = Network_Multicast_init();
  if(ret != SDDS_RT_OK){
    sys_task_terminate();
    sys_abort();
  }
#endif
  return SDDS_RT_OK;
}

rc_t Network_Multicast_init()
{
  rc_t ret;

  net.fd_multi_conn = create_connection(PLATFORM_AUTOBEST_SDDS_PORT + PLATFORM_AUTOBEST_SDDS_BUILTIN_MULTICAST_PORT_OFF);
  if( net.fd_multi_conn == NULL){
    Log_error("error create_connection [multicast]\n");
    return SDDS_RT_FAIL;
  }
  ret = Network_Multicast_joinMulticastGroup(PLATFORM_AUTOBEST_SDDS_BUILTIN_MULTICAST_ADDRESS);
  if (ret != SDDS_RT_OK) {
    Log_error("unable to join group: %s\n", PLATFORM_AUTOBEST_SDDS_BUILTIN_MULTICAST_ADDRESS);
    return SDDS_RT_FAIL;
  }
  ret = Network_Multicast_joinMulticastGroup(PLATFORM_AUTOBEST_SDDS_BUILTIN_MULTICAST_PARTICIPANT_ADDRESS);
  if (ret != SDDS_RT_OK) {
    Log_error("unable to join group: %s\n", PLATFORM_AUTOBEST_SDDS_BUILTIN_MULTICAST_PARTICIPANT_ADDRESS);
    return SDDS_RT_FAIL;
  }
  ret = Network_Multicast_joinMulticastGroup(PLATFORM_AUTOBEST_SDDS_BUILTIN_MULTICAST_SUB_PUB_ADDRESS);
  if (ret != SDDS_RT_OK) {
    Log_error("unable to join group: %s\n", PLATFORM_AUTOBEST_SDDS_BUILTIN_MULTICAST_SUB_PUB_ADDRESS);
    return SDDS_RT_FAIL;
  }
  ret = Network_Multicast_joinMulticastGroup(PLATFORM_AUTOBEST_SDDS_BUILTIN_MULTICAST_TOPIC_ADDRESS);
  if (ret != SDDS_RT_OK) {
    Log_error("unable to join group: %s\n", PLATFORM_AUTOBEST_SDDS_BUILTIN_MULTICAST_TOPIC_ADDRESS);
    return SDDS_RT_FAIL;
  }

  NetBuffRef_init(&multiInBuff);
  Network_createMulticastLocator(&multiInBuff.addr);

  // set up a thread to read from the udp socket [multicast]
  net.multiRecvThread = Thread_create();
  if(net.multiRecvThread == NULL){
    sys_task_terminate();
    sys_abort();
  }
  ret = Thread_init(net.multiRecvThread, recvLoop, (void *)&multiInBuff);
  if(ret != SDDS_RT_OK){
    Log_error("unabele to create task for recving data via multicast\n");
    sys_task_terminate();
    sys_abort();
  }
  return SDDS_RT_OK;
}

rc_t Network_Multicast_joinMulticastGroup(char *group) 
{
  struct netif* netif = netif_default;
  ip_addr_t multi_addr;
  //ip_addr_t if_addr;
  err_t err;
  int ret = inet6_aton(group, &multi_addr);
  if(ret != 1){
    Log_error("No vaild IPv6: %s\n", group);
    return SDDS_RT_FAIL;
  }
  multi_addr.type = IPADDR_TYPE_V6;
  err = netconn_join_leave_group(net.fd_multi_conn, &multi_addr, &netif->ip6_addr[0], NETCONN_JOIN);
  if (err != ERR_OK)
  {
    Log_error("unable to join group: %s\n", lwip_strerr(err));
    return SDDS_RT_FAIL;
  }
  return SDDS_RT_OK;
}

static struct netconn* create_connection(int port)
{
  struct netconn* conn =  NULL;
  ip_addr_t addr;
#if PLATFORM_AUTOBEST_SDDS_PROTOCOL == AF_INET
  int ret = inet_aton(PLATFORM_AUTOBEST_SDDS_ADDRESS, &addr);
  if(ret != 1){
    Log_error("no vaild IP: %s\n", PLATFORM_AUTOBEST_SDDS_ADDRESS);
    return NULL;
  }
  conn = netconn_new(NETCONN_UDP);
#elif PLATFORM_AUTOBEST_SDDS_PROTOCOL == AF_INET6
  int ret = inet6_aton(PLATFORM_AUTOBEST_SDDS_ADDRESS, &addr);
  if(ret != 1){
    netconn_delete(conn);
    Log_error("No vaild IPv6: %s\n", PLATFORM_AUTOBEST_SDDS_ADDRESS);
    return NULL;
  }
  addr.type = IPADDR_TYPE_V6;
  conn = netconn_new(NETCONN_UDP_IPV6);
#else
#error "Only AF_INET and AF_INET6 are understood protocols."
#endif
  if(conn == NULL){
    Log_error("Unable to create connection\n");
    return NULL;
  }
  err_t err = netconn_bind(conn, &addr, port);
  if(err != ERR_OK){
    netconn_delete(conn);
    Log_error("Unable to bind connection: %s\n", lwip_strerr(err));
    return NULL;
  }

  return conn;
}

void *recvLoop(void *netBuff)
{
    NetBuffRef_t* buff = (NetBuffRef_t*) netBuff;
    struct netconn* conn; // receive socket
    unsigned int conn_type; // broad or multicast
    struct netbuf* lwip_netbuf;
    void* data;
    u16_t recv_size;
    err_t err;
    int port;

    // Check the dummy locator for uni or multicast socket
    struct Locator_t *l = (struct Locator_t *) buff->addr;
    conn_type = l->type;
    
    if(conn_type == SDDS_LOCATOR_TYPE_MULTI) 
    {
        conn = net.fd_multi_conn; 
        //port = PLATFORM_AUTOBEST_SDDS_PORT + PLATFORM_AUTOBEST_SDDS_BUILTIN_MULTICAST_PORT_OFF;
    }
    else if(conn_type == SDDS_LOCATOR_TYPE_UNI)
    {
        conn = net.fd_uni_conn;
        //port = PLATFORM_AUTOBEST_SDDS_PORT;
    }

    while (true)
    {
      //reset the buffer
      NetBuffRef_renew(buff);
      err = netconn_recv(conn, &lwip_netbuf);
      if(err != ERR_OK ){
        Log_error("Error checking if a netbuffer was recvied\n");
        sys_task_terminate();
        sys_abort();
      }
      err = netbuf_data(lwip_netbuf, &data, &recv_size);
      if(err != ERR_OK ){
        Log_error("Error while read data out the netbuffer\n");
        sys_task_terminate();
        sys_abort();
      }
      if(recv_size == 0){
        Log_debug( "No data!\n");
        netbuf_delete(lwip_netbuf);
        continue;
      }
      Log_debug("[%u]%i bytes empfangen\n", conn_type, (int) recv_size);
      
      memcpy(buff->buff_start, data, recv_size);
      AutobestLocator_t sloc;
      memcpy(&(sloc.addr_storage), netbuf_fromaddr(lwip_netbuf), sizeof(ip_addr_t));
      port = netbuf_fromport(lwip_netbuf);
      sloc.port = port;
#if PLATFORM_AUTOBEST_SDDS_PROTOCOL == AF_INET6
      sloc.addr_storage.type =  IPADDR_TYPE_V6;
#endif
      Locator loc;
      if (LocatorDB_findLocator((Locator)&sloc, &loc) != SDDS_RT_OK)
      {
        // not found we need a new one
        if (LocatorDB_newLocator(&loc) != SDDS_RT_OK)
        {
          netbuf_delete(lwip_netbuf);
          continue;
        }
        memcpy(&((AutobestLocator_t *)loc)->addr_storage, &lwip_netbuf->addr, sizeof(ip_addr_t));
        ((AutobestLocator_t *)loc)->port = port;
      }

      netbuf_delete(lwip_netbuf);
      // up ref counter
      Locator_upRef(loc);
  
      loc->isEmpty = false;
      loc->isSender = true;
      loc->type = conn_type;
         
      buff->addr = loc;
      /*if(conn_type == SDDS_LOCATOR_TYPE_MULTI){
        multiInBuff.addr = loc;
      }
      else if(conn_type == SDDS_LOCATOR_TYPE_UNI){
        inBuff.addr = loc;
      }*/
      
      DataSink_processFrame(buff);
      LocatorDB_freeLocator(loc);
    }    
    return SDDS_RT_OK;
}

rc_t Network_send(NetBuffRef_t* buff) {
  struct netconn* conn;
  unsigned int conn_type;
  unsigned int port = 0;
  struct netbuf* netbuf = NULL;
  void* data = NULL; 

  /*printf("========== send NetBuff ==========\n");
  NetBuffRef_print(buff);
  printf("========== END NetBuff =======\n");*/

  // Check the locator for uni or multicast socket
  struct Locator_t *loc = (struct Locator_t *) buff->addr;
  conn_type = loc->type;
  // add locator to the netbuffref
  if(conn_type == SDDS_LOCATOR_TYPE_MULTI) {
    conn = net.fd_multi_conn;
  }
  else if(conn_type == SDDS_LOCATOR_TYPE_UNI){
    conn = net.fd_uni_conn;
  }

  while (loc != NULL ) {
    err_t err;
    ip_addr_t* addr = (ip_addr_t*) &(((AutobestLocator_t *) loc)->addr_storage);
    netbuf = netbuf_new();
    if(netbuf == NULL){
      Log_error("Can't alloc netbuffer:\n");
      return SDDS_RT_FAIL;
    }
    data = netbuf_alloc(netbuf, buff->curPos); 
    if(data == NULL){
      Log_error("Can't alloc databuffer:\n");
      return SDDS_RT_FAIL;
    }
    memcpy (data, buff->buff_start, buff->curPos);
    err = netconn_sendto(conn, netbuf, addr, ((AutobestLocator_t *) loc)->port);
    netbuf_delete(netbuf); 
    if (err != ERR_OK ) {
      Log_error("Can't send udp paket: %s\n", lwip_strerr(err));
      return SDDS_RT_FAIL;
    }
#ifdef UTILS_DEBUG
    char a[PLATFORM_AUTOBEST_IPV6_MAX_CHAR_LEN];
    Locator_getAddress(loc, a);
    Log_debug("Sendet to %s\n", a);
#endif
    loc = loc->next;

  }
  return SDDS_RT_OK;
}

void Network_recvFrameHandler(Network _this)
{
  (void)_this;
}


rc_t Network_getFrameBuff(NetFrameBuff* buff)
{
  size_t size = SDDS_NET_MAX_BUF_SIZE * sizeof(byte_t);
  size += sizeof(struct NetFrameBuff_t);

  *buff = Memory_alloc(size);

  if (*buff == NULL){
    return SDDS_RT_NOMEM;
  }
  memset(*buff, 0, size);

  (*buff)->size = SDDS_NET_MAX_BUF_SIZE;
  return SDDS_RT_OK;
}

rc_t Network_getPayloadBegin(size_t* startByte)
{
  // payload starts at the beginning of the buffer, address is provided
  // Separately
  // buffer starts after the struct
  *startByte = sizeof(struct NetFrameBuff_t);

  return SDDS_RT_OK;
}
size_t Network_locSize(void)
{
    return sizeof(AutobestLocator_t);
}

rc_t Network_setAddressToLocator(Locator loc, char* addr) 
{
  if (loc == NULL || addr == NULL) 
  {
    return SDDS_RT_BAD_PARAMETER;
  }
  AutobestLocator_t* aloc = (AutobestLocator_t*) loc;
#if PLATFORM_AUTOBEST_SDDS_PROTOCOL == AF_INET
  int ret = inet_aton(addr, &aloc->addr_storage);
#elif PLATFORM_AUTOBEST_SDDS_PROTOCOL == AF_INET6
  int ret = inet6_aton(addr, &aloc->addr_storage);
  aloc->addr_storage.type = IPADDR_TYPE_V6;
#else
#error "Only AF_INET and AF_INET6 are understood protocols."
#endif
  if(ret != 1)
  {
    Log_error("Faild to convert address %s\n", addr);
    return SDDS_RT_FAIL;
  }
  aloc->port = PLATFORM_AUTOBEST_SDDS_PORT;
#ifdef UTILS_DEBUG
  char a[PLATFORM_AUTOBEST_IPV6_MAX_CHAR_LEN];
  ret = Locator_getAddress(loc, a);
  Log_debug("Connection from %s\n", a);
#endif
  return SDDS_RT_OK;
}

rc_t Network_setMulticastAddressToLocator(Locator loc, char* addr) 
{
  if (loc == NULL || addr == NULL) 
  {
    return SDDS_RT_BAD_PARAMETER;
  }
  AutobestLocator_t* aloc = (AutobestLocator_t*) loc;


#if PLATFORM_AUTOBEST_SDDS_PROTOCOL == AF_INET
  int ret = inet_aton(addr, &aloc->addr_storage);
#elif PLATFORM_AUTOBEST_SDDS_PROTOCOL == AF_INET6
  int ret = inet6_aton(addr, &aloc->addr_storage);
  aloc->addr_storage.type = IPADDR_TYPE_V6;
#else
#error "Only AF_INET and AF_INET6 are understood protocols."
#endif
  if(ret != 1)
  {
    Log_error("Faild to convert address: %s \n", addr);
    return SDDS_RT_FAIL;
  }
  aloc->port = PLATFORM_AUTOBEST_SDDS_PORT + PLATFORM_AUTOBEST_SDDS_BUILTIN_MULTICAST_PORT_OFF;
  char* address_buffer = inet6_ntoa(aloc->addr_storage);
  Log_debug("created a locator for [%s]\n", address_buffer);
  return SDDS_RT_OK;
}

rc_t Network_createLocator(Locator* loc)
{
  *loc = Memory_alloc(sizeof(AutobestLocator_t));
  if (*loc == NULL)
  {
    return SDDS_RT_NOMEM;
  }

  // set type for recvLoop
  (*loc)->type = SDDS_LOCATOR_TYPE_UNI;

  return Network_setAddressToLocator(*loc, PLATFORM_AUTOBEST_SDDS_ADDRESS);
}

rc_t Network_createMulticastLocator(Locator* loc) 
{
  *loc = Memory_alloc(sizeof(AutobestLocator_t));
  if(*loc == NULL) 
  {
      return SDDS_RT_NOMEM;
  }
  // set type for recvLoop
  (*loc)->type = SDDS_LOCATOR_TYPE_MULTI;
  return Network_setMulticastAddressToLocator(*loc, PLATFORM_AUTOBEST_SDDS_BUILTIN_MULTICAST_ADDRESS);
}

bool_t Locator_isEqual(Locator l1, Locator l2)
{
  AutobestLocator_t* a = (AutobestLocator_t *)l1;
  AutobestLocator_t* b = (AutobestLocator_t *)l2;

  ip_addr_t *addr[2];

  addr[0] = (ip_addr_t*)&a->addr_storage;
  addr[1] = (ip_addr_t*)&b->addr_storage;

  if (memcmp(&addr[0], &addr[1], sizeof(ip_addr_t)) == 0)
  {
    return true;
  } 
  return false;
}


rc_t Locator_getAddress(Locator l, char *srcAddr) {
  AutobestLocator_t* aloc = (AutobestLocator_t*) l;
#if PLATFORM_AUTOBEST_SDDS_PROTOCOL == AF_INET
  char* ret = inet_ntoa(aloc->addr_storage);
#elif PLATFORM_AUTOBEST_SDDS_PROTOCOL == AF_INET6
  char* ret = inet6_ntoa(aloc->addr_storage);
#else
#error "Only AF_INET and AF_INET6 are understood protocols."
#endif
  memcpy(srcAddr, ret, strlen(ret)+1);
  //Log_debug("%s | %s\n", ret, srcAddr);
  if(ret == NULL) 
  {
    return SDDS_RT_FAIL;
  }
  return SDDS_RT_OK;
}
