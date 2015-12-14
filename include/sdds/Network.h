/*
 * =====================================================================================
 *
 *       Filename:  Network.h
 *
 *    Description:  Header file for the Network abstractation
 *
 *        Version:  1.0
 *        Created:  22.02.2010 15:53:57
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */

#ifndef  NETWORK_H_INC
#define  NETWORK_H_INC

struct Network_t;

typedef struct Network_t* Network;

rc_t
Network_init(void);

size_t
Network_size(void);

rc_t
Network_send(NetBuffRef_t* buff);

void
Network_recvFrameHandler(Network _this);

rc_t
Network_getFrameBuff(NetFrameBuff* buff);
rc_t
Network_getPayloadBegin(size_t* startByte);

rc_t
Network_createLocator(Locator_t** loc);
rc_t
Network_createMulticastLocator(Locator_t** loc);
rc_t
Network_setMulticastAddressToLocator(Locator_t* loc, char* addr);

size_t
Network_locSize(void);

rc_t
Network_set_locator_endpoint(Locator_t* loc, char* addr, int port);

rc_t
Network_setAddressToLocator(Locator_t* loc, char* addr);

rc_t
Network_getSrcAddr(char* addr, int addr_len, char* port,
                   int port_len);

void
Locator_clone(Locator_t* src, Locator_t* dst);

#endif   /* ----- #ifndef NETWORK_H_INC  ----- */
