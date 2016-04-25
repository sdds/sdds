#include "sDDS.h"

#if defined(FEATURE_SDDS_SECURITY_ENABLED) && defined(SDDS_SECURITY_KDC)

static MessageIdentity msgid;
static HandshakeHandle g_handles[SDDS_SECURITY_HANDSHAKE_MAX];
List_t* ac_list;

rc_t 
Security_kdc_init() {
  ac_list = List_initLinkedList();  
  return SDDS_RT_OK;
}

rc_t 
Security_kdc() {

  DDS_S_Result_t res = VALIDATION_FAILED;
  DDS_ReturnCode_t r;
  SecurityException ex;
  HandshakeHandle *handle;  
  HandshakeMessageToken msg_tok_out;
  HandshakeMessageToken msg_tok_in;
  DDS_ParticipantStatelessMessage msg;
  DDS_ParticipantStatelessMessage *msg_ptr = &msg;
  DDS_SampleInfo info;

  while(1) {
 
    Thread_sleep(NULL, SDDS_SECURITY_RECEIVE_SLEEP_SEC);
    r = DDS_ParticipantStatelessMessageDataReader_take_next_sample(
                                 g_ParticipantStatelessMessage_reader,
                                 &msg_ptr,
                                 &info);
    if(r != DDS_RETCODE_OK) {
      continue;
    }    
    msg_tok_in = msg.message_data;
        
    if(strcmp(msg.message_data.class_id, SDDS_SECURITY_CLASS_AUTH_REQ) == 0) {
      Log_debug("request from node\n");
      res = DDS_Security_Authentication_begin_handshake_reply(
        handle,
        &msg_tok_out, 
        &msg_tok_in, 
        &msg.key, 
        NULL, 
        &ex 
      );
    } else if(strcmp(msg.message_data.class_id, SDDS_SECURITY_CLASS_AUTH_REP) == 0) {
      Log_debug("reply from node\n");     
      if((handle = Security_get_handshake_handle(&msg.key)) == NULL) {
        Log_error("can't get handle\n");
        continue;
      }      
      res = DDS_Security_Authentication_process_handshake(
        &msg_tok_out,
        &msg_tok_in,
        handle,
        &ex
      );
    } else {
      continue;
    }

    switch(res) {

      case VALIDATION_OK:
        Log_debug("VALIDATION_OK\n");
      break;

      case VALIDATION_FAILED:
        Log_error("VALIDATION_FAILED\n");
        Security_cleanup_handshake_handle(handle);
      break;

      case VALIDATION_PENDING_HANDSHAKE_MESSAGE: 
        // send reply token
        msg.msgid = msgid++;
        strcpy((char *) msg.message_class_id, GMCLASSID_SECURITY_AUTH_HANDSHAKE);  
        msg.message_data = msg_tok_out;
        r = DDS_ParticipantStatelessMessageDataWriter_write(g_ParticipantStatelessMessage_writer,
                                                            &msg,
                                                            NULL);  
        if(r == DDS_RETCODE_OK) {
          Log_debug("sent reply token\n");
        } else {
          Log_error("failed to send reply\n");
          Security_cleanup_handshake_handle(handle);
        }
      break;  

      case VALIDATION_OK_FINAL_MESSAGE:
        // send final token
        msg.msgid = msgid++;
        strcpy((char *) msg.message_class_id, GMCLASSID_SECURITY_AUTH_HANDSHAKE);            
        msg.message_data = msg_tok_out;
        r = DDS_ParticipantStatelessMessageDataWriter_write(g_ParticipantStatelessMessage_writer,
                                                            &msg,
                                                            NULL);  
        if(r == DDS_RETCODE_OK) {
          Log_debug("VALIDATION_OK_FINAL_MESSAGE\n");
        } else {
          Log_error("failed to send final token\n");
          Security_cleanup_handshake_handle(handle);
        }

        Security_send_crypto_tokens(handle);

      break;
    }

  }

  return SDDS_RT_OK;

}

rc_t
Security_send_crypto_tokens(HandshakeHandle *h) {

  DDS_ReturnCode_t r;
  DDS_ParticipantVolatileMessage msg;
  Ac_topic *rule = ac_list->first_fn(ac_list);
  List_t *apps;
  char *app;
  uint8_t key_material[SDDS_SECURITY_KDF_KEY_BYTES];
  uint8_t iv[SDDS_SECURITY_IV_SIZE];
  uint8_t mac[XCBC_MAC_SIZE];

  memset(&msg, 0, sizeof(msg));

  getRandomBytes(iv, SDDS_SECURITY_IV_SIZE);

  while(rule) {
    app = rule->applist->first_fn(rule->applist);
    while(app) {
      //printf("%s %s\n", app, h->info.uid);
      if(strncmp(app, h->info.uid, SDDS_SECURITY_USER_ID_STRLEN) == 0) {      
        Log_debug("send key for topic %d to %.8s: ", rule->tid, h->info.uid);      
        Security_print_key(rule->key_material, SDDS_SECURITY_KDF_KEY_BYTES);

        memcpy(key_material, rule->key_material, sizeof(rule->key_material));
        Security_aes_ctr(iv, h->info.key_material, key_material, sizeof(key_material));

        strcpy(msg.message_class_id, GMCLASSID_SECURITY_PARTICIPANT_CRYPTO_TOKENS);
        msg.key = h->node;
        strcpy(msg.message_data.props[0].key, SDDS_SECURITY_PROP_TID);
        memcpy(msg.message_data.props[0].value, &rule->tid, sizeof(rule->tid)); 
        strcpy(msg.message_data.props[1].key, SDDS_SECURITY_PROP_KEY_MATERIAL);
        memcpy(msg.message_data.props[1].value, key_material, sizeof(key_material)); 
        strcpy(msg.message_data.props[2].key, SDDS_SECURITY_PROP_IV);
        memcpy(msg.message_data.props[2].value, iv, sizeof(iv)); 

        // calculate xcbc mac
        Security_aes_xcbc_mac(h->info.key_material + AES_128_KEY_LENGTH, 
                              (uint8_t *) &msg, sizeof(msg), 
                              mac);

        memcpy(msg.message_data.props[2].value + sizeof(iv), mac, sizeof(mac)); 

        r = DDS_ParticipantVolatileMessageDataWriter_write(g_ParticipantVolatileMessage_writer,
                                                            &msg,
                                                            NULL);  
        if(r == DDS_RETCODE_OK) {
          Log_debug("sent crypto token\n");
        } else {
          Log_error("failed to send crypto token\n");
        }       
      }

      app = rule->applist->next_fn(rule->applist);
    }
    rule = ac_list->next_fn(ac_list);
  }
  
}

rc_t
Security_kdc_add_rule(uint16_t tid, char *aid) {

  Ac_topic *rule = ac_list->first_fn(ac_list);
  uint8_t found = 0;

  while(rule) {
    if(rule->tid == tid) {
      found = 1;
      rule->applist->add_fn(rule->applist, strdup(aid));      
    }
    rule = ac_list->next_fn(ac_list);
  }

  if(!found) {
    rule = Memory_alloc(sizeof(Ac_topic));
    rule->tid = tid;
    rule->applist = List_initLinkedList();
    rule->applist->add_fn(rule->applist, strdup(aid));
    getRandomBytes(rule->key_material, SDDS_SECURITY_KDF_KEY_BYTES);
    ac_list->add_fn(ac_list, rule);
  }

  return SDDS_RT_OK;

}

HandshakeHandle*
Security_get_handshake_handle(IdentityHandle *node) {

  int i;
  for(i = 0; i < SDDS_SECURITY_HANDSHAKE_MAX; i++) {
    if(g_handles[i].node == *node) {
      return &g_handles[i];
    }
  }

  return NULL;

}

HandshakeHandle*
Security_new_handshake_handle(IdentityHandle *node) {

  int i;
  for(i = 0; i < SDDS_SECURITY_HANDSHAKE_MAX; i++) {
    if(g_handles[i].node == 0) {
      g_handles[i].node = *node;
      return &g_handles[i];
    }
  }

  return NULL;

}

void
Security_cleanup_handshake_handle(HandshakeHandle *h) {
  memset(h, 0, sizeof(HandshakeHandle));
}

#endif
