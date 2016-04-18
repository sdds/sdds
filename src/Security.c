#include "sDDS.h"

#ifdef FEATURE_SDDS_SECURITY_ENABLED

static MessageIdentity msgid;
static HandshakeHandle g_handle;
List_t* key_list;

rc_t 
Security_init() {

#ifndef SDDS_SECURITY_KDC

  if(Security_auth() == SDDS_RT_FAIL) {
    return SDDS_RT_FAIL;
  }

  key_list = List_initLinkedList();  
  Security_receive_key();
  
#endif

  return SDDS_RT_OK;

}

Ac_topic*
Security_lookup_key(uint16_t tid) {

  Ac_topic *key = key_list->first_fn(key_list);
  while(key) {
    if(key->tid == tid) {
      return key;
    }
    key = key_list->next_fn(key_list);
  }
  
  return NULL;

}

rc_t 
Security_receive_key() {

  DDS_ReturnCode_t r;  
  DDS_ParticipantVolatileMessage msg;  
  DDS_ParticipantVolatileMessage *msg_ptr = &msg;
  DDS_SampleInfo info;
  uint16_t tid;  
  uint8_t key_material[SDDS_SECURITY_KDF_KEY_BYTES];
  uint8_t iv[SDDS_SECURITY_IV_SIZE];
  uint8_t mac[XCBC_MAC_SIZE];
  uint8_t remote_mac[XCBC_MAC_SIZE];
  uint8_t *ptr = (uint8_t *) &msg;

  memset(&msg, 0, sizeof(msg));

  r = DDS_ParticipantVolatileMessageDataReader_take_next_sample(
                               g_ParticipantVolatileMessage_reader,
                               &msg_ptr,
                               &info);

  if(r == DDS_RETCODE_OK) {
    printf("received something\n");
  }

  if(r != DDS_RETCODE_OK 
      || msg.key != BuiltinTopic_participantID 
      || strcmp(msg.message_class_id, GMCLASSID_SECURITY_PARTICIPANT_CRYPTO_TOKENS) != 0) {
    printf("failed to receive crypto token\n");
    return SDDS_RT_FAIL;
  }  

  memcpy(remote_mac, msg.message_data.props[2].value + sizeof(iv), sizeof(remote_mac));
  memset(msg.message_data.props[2].value + sizeof(iv), 0, sizeof(remote_mac));

  // calculate xcbc mac
  Security_aes_xcbc_mac(g_handle.info.key_material + AES_128_KEY_LENGTH, 
                        (uint8_t *) &msg, sizeof(msg), 
                        mac);
 
  if(memcmp(remote_mac, mac, sizeof(mac)) == 0) {
    printf("received key, mac is ok\n");
  } else {
    return SDDS_RT_FAIL;
  }

  memcpy(&tid, msg.message_data.props[0].value, sizeof(tid));
  memcpy(key_material, msg.message_data.props[1].value, sizeof(key_material));
  memcpy(iv, msg.message_data.props[2].value, sizeof(iv));

  Security_aes_ctr(iv, g_handle.info.key_material, key_material, sizeof(key_material));

  printf("key for topic %d: ", tid);
  Security_print_key(key_material, SDDS_SECURITY_KDF_KEY_BYTES);

  Ac_topic *key = Memory_alloc(sizeof(Ac_topic));
  key->tid = tid;
  memcpy(key->key_material, key_material, sizeof(key_material));
  key_list->add_fn(key_list, key);

  return SDDS_RT_OK;

}

rc_t 
Security_auth() {

  DDS_S_Result_t res = VALIDATION_FAILED;
  DDS_ReturnCode_t r;  
  SecurityException ex;
  HandshakeMessageToken msg_tok_out;
  HandshakeMessageToken msg_tok_in;
  DDS_ParticipantStatelessMessage msg;
  DDS_ParticipantStatelessMessage *msg_ptr = &msg;
  DDS_SampleInfo info;
  int i, j;
  
  res = DDS_Security_Authentication_begin_handshake_request(
    &g_handle,
    &msg_tok_out, 
    &msg_tok_in, 
    NULL, 
    NULL, 
    &ex 
  );

  for(i = 0; i < SDDS_MAX_AUTH_ITERATIONS; i++) {

    switch(res) {

      case VALIDATION_OK:
        printf("VALIDATION_OK\n");
        return SDDS_RT_OK;
      break;

      case VALIDATION_FAILED:
        printf("VALIDATION_FAILED\n");        
        return SDDS_RT_FAIL;
      break;

      case VALIDATION_PENDING_HANDSHAKE_MESSAGE:      
        // send token
        msg.msgid = msgid++;
        msg.key = BuiltinTopic_participantID;
        strcpy((char *) msg.message_class_id, GMCLASSID_SECURITY_AUTH_HANDSHAKE);  
        
        msg.message_data = msg_tok_out;
        r = DDS_ParticipantStatelessMessageDataWriter_write(g_ParticipantStatelessMessage_writer,
                                                            &msg,
                                                            NULL);  
        if(r == DDS_RETCODE_OK) {
          // OK
        } else {
          printf("failed to send token\n");
          res = VALIDATION_FAILED;
          break;
        }
        
        // wait for a reply from kdc
        for(j = 0; j < 3; j++) {
          Thread_sleep(NULL, SDDS_SECURITY_RECEIVE_SLEEP_SEC);
          r = DDS_ParticipantStatelessMessageDataReader_take_next_sample(
                                                           g_ParticipantStatelessMessage_reader,
                                                           &msg_ptr,
                                                           &info);
          if(r == DDS_RETCODE_OK && msg.key == BuiltinTopic_participantID) {
            printf("received reply from kdc\n");
            msg_tok_in = msg.message_data;
            res = DDS_Security_Authentication_process_handshake(
	            &msg_tok_out,
	            &msg_tok_in,
	            &g_handle,
	            &ex
            );
            break;
          }
         
          continue;
                          
        }
      break;

      case VALIDATION_OK_FINAL_MESSAGE:
        // send final token
        msg.msgid = msgid++;
        msg.key = BuiltinTopic_participantID;
        strcpy((char *) msg.message_class_id, GMCLASSID_SECURITY_AUTH_HANDSHAKE);  
        
        msg.message_data = msg_tok_out;
        r = DDS_ParticipantStatelessMessageDataWriter_write(g_ParticipantStatelessMessage_writer,
                                                            &msg,
                                                            NULL);  
        if(r == DDS_RETCODE_OK) {
          res = VALIDATION_OK;
        } else {
          printf("failed to send final token\n");
          res = VALIDATION_FAILED;
        }
      break;

    }
  }

  return SDDS_RT_FAIL;

}

DDS_S_Result_t 
DDS_Security_Authentication_begin_handshake_request(
	HandshakeHandle *handshake_handle, /* inout (fixed length) */
	HandshakeMessageToken *handshake_message_out, /* in (variable length) */
	HandshakeMessageToken *handshake_message_in, /* in (variable length) */
	IdentityHandle *initiator_identity_handle, /* in (fixed length) */
	IdentityHandle *replier_identity_handle, /* in (fixed length) */
	SecurityException *ex /* inout (variable length) */
) {

  memset(handshake_handle, 0, sizeof(HandshakeHandle));
  handshake_handle->state = SDDS_SECURITY_HANDSHAKE_STATE_0;

  // start id and public key in handshake_message_out
  strcpy(handshake_message_out->class_id, SDDS_SECURITY_CLASS_AUTH_REQ);

  strcpy(handshake_message_out->props[0].key, SDDS_SECURITY_PROP_ID);

  // USER,VERSION,TIMESTAMP,CIPHER_SUITE
  sprintf(handshake_message_out->props[0].value, "%s,%02x,%08x,%04x,", 
          SDDS_SECURITY_USER_ID, SDDS_SECURITY_VERSION, 
          SDDS_SECURITY_TIMESTAMP, SDDS_SECURITY_CIPHER_SUITE); 

  strcpy(handshake_message_out->props[1].key, SDDS_SECURITY_PROP_PUB_KEY_X); 
  Security_get_bytes(handshake_message_out->props[1].value, SDDS_SECURITY_USER_PUBLIC_KEY_X, NUM_ECC_DIGITS);

  strcpy(handshake_message_out->props[2].key, SDDS_SECURITY_PROP_PUB_KEY_Y); 
  Security_get_bytes(handshake_message_out->props[2].value, SDDS_SECURITY_USER_PUBLIC_KEY_Y, NUM_ECC_DIGITS);

  return VALIDATION_PENDING_HANDSHAKE_MESSAGE;

}

DDS_S_Result_t DDS_Security_Authentication_begin_handshake_reply(
	HandshakeHandle *handshake_handle, /* inout (fixed length) */
	HandshakeMessageToken *handshake_message_out, /* inout (variable length) */
	HandshakeMessageToken *handshake_message_in, /* in (variable length) */
	IdentityHandle *initiator_identity_handle, /* in (fixed length) */
	IdentityHandle *replier_identity_handle, /* in (fixed length) */
	SecurityException *ex /* inout (variable length) */
) {

#ifdef SDDS_SECURITY_KDC
  handshake_handle = Security_new_handshake_handle(initiator_identity_handle);
#endif

  if(handshake_handle == NULL) {
    return VALIDATION_FAILED;
  }

  handshake_handle->state = SDDS_SECURITY_HANDSHAKE_STATE_1;

  // fetch remote id and public key from handshake_message_in
  strncpy(handshake_handle->info.uid, handshake_message_in->props[0].value, sizeof(handshake_handle->info.uid));
  memcpy(handshake_handle->info.public_key.x, handshake_message_in->props[1].value, sizeof(handshake_handle->info.public_key.x));
  memcpy(handshake_handle->info.public_key.y, handshake_message_in->props[2].value, sizeof(handshake_handle->info.public_key.y));

  // reply with id and public key in handshake_message_out
  strcpy(handshake_message_out->props[0].key, SDDS_SECURITY_PROP_ID);

  // USER,VERSION,TIMESTAMP,CIPHER_SUITE
  sprintf(handshake_message_out->props[0].value, "%s,%02x,%08x,%04x,", 
          SDDS_SECURITY_USER_ID, SDDS_SECURITY_VERSION, 
          SDDS_SECURITY_TIMESTAMP, SDDS_SECURITY_CIPHER_SUITE); 

  strcpy(handshake_message_out->props[1].key, SDDS_SECURITY_PROP_PUB_KEY_X); 
  Security_get_bytes(handshake_message_out->props[1].value, SDDS_SECURITY_USER_PUBLIC_KEY_X, NUM_ECC_DIGITS);
  strcpy(handshake_message_out->props[2].key, SDDS_SECURITY_PROP_PUB_KEY_Y); 
  Security_get_bytes(handshake_message_out->props[2].value, SDDS_SECURITY_USER_PUBLIC_KEY_Y, NUM_ECC_DIGITS);
  return VALIDATION_PENDING_HANDSHAKE_MESSAGE;

}

DDS_S_Result_t 
DDS_Security_Authentication_process_handshake(
	HandshakeMessageToken *handshake_message_out,
	HandshakeMessageToken *handshake_message_in,
	HandshakeHandle *handshake_handle,
	SecurityException *ex
) {
  
  DDS_S_Result_t res = VALIDATION_FAILED;
  uint8_t mactag_data[NUM_ECC_DIGITS]; 
  uint8_t *mactag_data_offset;

  switch (handshake_handle->state) {

    case SDDS_SECURITY_HANDSHAKE_STATE_0:

      // fetch remote id and public key from handshake_message_in
      strncpy(handshake_handle->info.uid, handshake_message_in->props[0].value, sizeof(handshake_handle->info.uid));
      memcpy(handshake_handle->info.public_key.x, handshake_message_in->props[1].value, sizeof(handshake_handle->info.public_key.x));
      memcpy(handshake_handle->info.public_key.y, handshake_message_in->props[2].value, sizeof(handshake_handle->info.public_key.y));

      // reply with signature and nonce in handshake_message_out
      getRandomBytes(handshake_handle->info.nonce, sizeof(handshake_handle->info.nonce));
      strcpy(handshake_message_out->class_id, SDDS_SECURITY_CLASS_AUTH_REP);

      strcpy(handshake_message_out->props[0].key, SDDS_SECURITY_PROP_SIG_R); 
      Security_get_bytes(handshake_message_out->props[0].value, SDDS_SECURITY_USER_CERT_SIG_R, NUM_ECC_DIGITS);

      strcpy(handshake_message_out->props[1].key, SDDS_SECURITY_PROP_SIG_S); 
      Security_get_bytes(handshake_message_out->props[1].value, SDDS_SECURITY_USER_CERT_SIG_S, NUM_ECC_DIGITS);

      strcpy(handshake_message_out->props[2].key, SDDS_SECURITY_PROP_NONCE); 
      memcpy(handshake_message_out->props[2].value, handshake_handle->info.nonce, sizeof(handshake_handle->info.nonce));

      handshake_handle->state = SDDS_SECURITY_HANDSHAKE_STATE_1;
      res = VALIDATION_PENDING_HANDSHAKE_MESSAGE;
    break;

    case SDDS_SECURITY_HANDSHAKE_STATE_1:

      // fetch remote signature and nonce from handshake_message_in
      memcpy(handshake_handle->info.signature_r, handshake_message_in->props[0].value, sizeof(handshake_handle->info.signature_r));
      memcpy(handshake_handle->info.signature_s, handshake_message_in->props[1].value, sizeof(handshake_handle->info.signature_s));
      memcpy(handshake_handle->info.remote_nonce, handshake_message_in->props[2].value, sizeof(handshake_handle->info.remote_nonce));

      // verify user certificate
      if(Security_verify_certificate(handshake_handle) == SDDS_RT_FAIL) {
        res = VALIDATION_FAILED;
        break;
      } else {
        printf("certificate ok\n");
      }

#ifdef SDDS_SECURITY_KDC
      // get nonce for kdc
      getRandomBytes(handshake_handle->info.nonce, sizeof(handshake_handle->info.nonce));
#endif

      // calculate shared secret and set key material
      if(Security_set_key_material(handshake_handle) == SDDS_RT_FAIL) {
        res = VALIDATION_FAILED;
        break;
      } else {
        printf("key material ok\n");
      }

#ifdef SDDS_SECURITY_KDC
      // reply with signature and nonce in handshake_message_out
      strcpy(handshake_message_out->class_id, SDDS_SECURITY_CLASS_AUTH_REP);

      strcpy(handshake_message_out->props[0].key, SDDS_SECURITY_PROP_SIG_R); 
      Security_get_bytes(handshake_message_out->props[0].value, SDDS_SECURITY_USER_CERT_SIG_R, NUM_ECC_DIGITS);

      strcpy(handshake_message_out->props[1].key, SDDS_SECURITY_PROP_SIG_S); 
      Security_get_bytes(handshake_message_out->props[1].value, SDDS_SECURITY_USER_CERT_SIG_S, NUM_ECC_DIGITS);

      strcpy(handshake_message_out->props[2].key, SDDS_SECURITY_PROP_NONCE); 
      memcpy(handshake_message_out->props[2].value, handshake_handle->info.nonce, sizeof(handshake_handle->info.nonce));
#else
      // reply with mactag in handshake_message_out
      strcpy(handshake_message_out->class_id, SDDS_SECURITY_CLASS_AUTH_REP);
      mactag_data_offset = mactag_data;
      memcpy(mactag_data_offset, handshake_handle->info.nonce, sizeof(handshake_handle->info.nonce));
      mactag_data_offset += sizeof(handshake_handle->info.nonce);
      memcpy(mactag_data_offset, handshake_handle->info.remote_nonce, sizeof(handshake_handle->info.remote_nonce));

      // calculate xcbc mac
      Security_aes_xcbc_mac(handshake_handle->info.key_material + AES_128_KEY_LENGTH, 
                            mactag_data, sizeof(mactag_data), 
                            handshake_handle->info.mactag);

      strcpy(handshake_message_out->props[0].key, SDDS_SECURITY_PROP_MACTAG); 
      memcpy(handshake_message_out->props[0].value, handshake_handle->info.mactag, sizeof(handshake_handle->info.mactag));
#endif
      handshake_handle->state = SDDS_SECURITY_HANDSHAKE_STATE_2;
      res = VALIDATION_PENDING_HANDSHAKE_MESSAGE;
    break;

    case SDDS_SECURITY_HANDSHAKE_STATE_2:
      // fetch remote mactag from handshake_message_in
      memcpy(handshake_handle->info.mactag, handshake_message_in->props[0].value, sizeof(handshake_handle->info.mactag));

      // verify mactag
      if(Security_verify_mactag(handshake_handle) == SDDS_RT_FAIL) {
        printf("wrong mactag\n");
        res = VALIDATION_FAILED;
        break;
      } else {
        printf("mactag ok\n");
#ifdef SDDS_SECURITY_KDC
        // reply with final mactag in handshake_message_out
        strcpy(handshake_message_out->class_id, SDDS_SECURITY_CLASS_AUTH_REP);
        mactag_data_offset = mactag_data;
        memcpy(mactag_data_offset, handshake_handle->info.nonce, sizeof(handshake_handle->info.nonce));
        mactag_data_offset += sizeof(handshake_handle->info.nonce);
        memcpy(mactag_data_offset, handshake_handle->info.remote_nonce, sizeof(handshake_handle->info.remote_nonce));

        // calculate xcbc mac
        Security_aes_xcbc_mac(handshake_handle->info.key_material + AES_128_KEY_LENGTH, 
                              mactag_data, sizeof(mactag_data), 
                              handshake_handle->info.mactag);

        strcpy(handshake_message_out->props[0].key, SDDS_SECURITY_PROP_MACTAG); 
        memcpy(handshake_message_out->props[0].value, handshake_handle->info.mactag, sizeof(handshake_handle->info.mactag));
        res = VALIDATION_OK_FINAL_MESSAGE;
#else 
        res = VALIDATION_OK;
#endif
      }
    break;

  }

  return res;

}

SharedSecretHandle
DDS_Security_Authentication_get_shared_secret(HandshakeHandle *h, SecurityException *ex) {
  return h->info.shared_secret;
}

rc_t
Security_set_key_material(HandshakeHandle *h) {

  uint8_t private_key[NUM_ECC_DIGITS];
  uint8_t random[NUM_ECC_DIGITS];

  Security_get_bytes(private_key, SDDS_SECURITY_USER_PRIVATE_KEY, NUM_ECC_DIGITS);
  getRandomBytes(random, sizeof(random));

  // calculate shared secret 
  if(!ecdh_shared_secret(h->info.shared_secret, &h->info.public_key, private_key, random)) {
    return SDDS_RT_FAIL;
  }

  // calculate key material (MAC- and AES-Key)
  Security_kdf(h);

  Security_print_key(h->info.key_material, SDDS_SECURITY_KDF_KEY_BYTES);

  return SDDS_RT_OK;

}

rc_t
Security_verify_certificate(HandshakeHandle *h) {

  EccPoint ca_publicKey;
  uint8_t hash[SHA224_HASH_BYTES];
  char str[NUM_ECC_DIGITS * 2 + 1];
	char cert[SDDS_SECURITY_CERT_STRLEN];
  char *p = cert;

	snprintf(p, SDDS_SECURITY_CERT_INFO_STRLEN + 1, 
                "%s", h->info.uid);
  p += SDDS_SECURITY_CERT_INFO_STRLEN;

  Security_get_string(str, h->info.public_key.x);
	snprintf(p, NUM_ECC_DIGITS * 2 + 2, "%s,", str);
  p += NUM_ECC_DIGITS * 2 + 1;

  Security_get_string(str, h->info.public_key.y);
	snprintf(p, NUM_ECC_DIGITS * 2 + 1, "%s", str);

  printf("check cert: %s\n", cert);

	memset(hash, 0, sizeof(hash));
	sha224(hash, cert, 8 * (SDDS_SECURITY_CERT_STRLEN - 1));

  Security_get_bytes(ca_publicKey.x, SDDS_SECURITY_CA_PUBLIC_KEY_X, NUM_ECC_DIGITS);
  Security_get_bytes(ca_publicKey.y, SDDS_SECURITY_CA_PUBLIC_KEY_Y, NUM_ECC_DIGITS);

  if(ecdsa_verify(&ca_publicKey, hash, h->info.signature_r, h->info.signature_s)) {
    return SDDS_RT_OK;
  }
  
  return SDDS_RT_FAIL;
  
}

rc_t
Security_verify_mactag(HandshakeHandle *h) {

  uint8_t mactag_data[NUM_ECC_DIGITS]; 
  uint8_t *mactag_data_offset;
  uint8_t mactag[XCBC_MAC_SIZE]; 

  mactag_data_offset = mactag_data;
  memcpy(mactag_data_offset, h->info.remote_nonce, sizeof(h->info.remote_nonce));
  mactag_data_offset += sizeof(h->info.remote_nonce);
  memcpy(mactag_data_offset, h->info.nonce, sizeof(h->info.nonce));

  // calculate xcbc mac
  Security_aes_xcbc_mac(h->info.key_material + AES_128_KEY_LENGTH, 
                        mactag_data, sizeof(mactag_data), 
                        mactag);

  if(memcmp(mactag, h->info.mactag, XCBC_MAC_SIZE)) {
    return SDDS_RT_FAIL;
  }

  return SDDS_RT_OK;

}



void 
Security_kdf(HandshakeHandle *h) {

  int reps = (SDDS_SECURITY_KDF_KEY_BYTES / SHA224_HASH_BYTES);
  int r = SDDS_SECURITY_KDF_KEY_BYTES % SHA224_HASH_BYTES;                           
  uint8_t hash[SHA224_HASH_BYTES];
  int size = sizeof(uint32_t) + sizeof(h->info.shared_secret) + sizeof(h->info.nonce);
  uint8_t data[size];

  if(r) {
    reps++;
  }

  for(uint32_t i = 0; i < reps; i++) {
   
    memcpy(data, &i, sizeof(i));
    memcpy(data + sizeof(i), h->info.shared_secret, sizeof(h->info.shared_secret));
#ifdef SDDS_SECURITY_KDC
    memcpy(data + sizeof(i) + sizeof(h->info.shared_secret), h->info.remote_nonce, sizeof(h->info.remote_nonce));
#else
    memcpy(data + sizeof(i) + sizeof(h->info.shared_secret), h->info.nonce, sizeof(h->info.nonce));
#endif

    sha224(hash, data, size*8);
      
    if(i == reps - 1 && r) {
      memcpy(h->info.key_material + (i*SHA224_HASH_BYTES), hash, r);    
    } else {      
      memcpy(h->info.key_material + (i*SHA224_HASH_BYTES), hash, SHA224_HASH_BYTES);    
    }

  }
  
}

void 
Security_aes_xcbc_mac(uint8_t aes_key[AES_128_KEY_LENGTH], uint8_t *data, uint8_t size, uint8_t mac[XCBC_MAC_SIZE]) {

  uint8_t key1[AES_128_KEY_LENGTH];
  uint8_t key2[AES_128_KEY_LENGTH];
  uint8_t key3[AES_128_KEY_LENGTH];

  uint8_t e[AES_128_KEY_LENGTH];
  uint8_t result[AES_128_KEY_LENGTH];
  uint8_t last_block[AES_128_KEY_LENGTH];

  uint8_t n, last, i, j;

  n = size / AES_128_KEY_LENGTH;
  last = size % AES_128_KEY_LENGTH;

  memset(e, 0, AES_128_KEY_LENGTH);
  memset(key1, XCBC_K1, AES_128_KEY_LENGTH);
  memset(key2, XCBC_K2, AES_128_KEY_LENGTH);
  memset(key3, XCBC_K3, AES_128_KEY_LENGTH);

  aes_128_set_padded_key(aes_key, AES_128_KEY_LENGTH);

  aes_128_padded_encrypt(key1, AES_128_KEY_LENGTH);
  aes_128_padded_encrypt(key2, AES_128_KEY_LENGTH);
  aes_128_padded_encrypt(key3, AES_128_KEY_LENGTH);

  aes_128_set_padded_key(key1, AES_128_KEY_LENGTH);

  for(i = 0; i < n; i++) {
    for(j = 0; j < AES_128_KEY_LENGTH; j++) {
      result[j] = data[(i*AES_128_KEY_LENGTH) + j] ^ e[j];      
    }    
    aes_128_padded_encrypt(result, AES_128_KEY_LENGTH);
    memcpy(e, result, AES_128_KEY_LENGTH);
  }

  if(!last) {   
    for(j = 0; j < AES_128_KEY_LENGTH; j++) {    
      result[j] = data[(n*AES_128_KEY_LENGTH) + j] ^ e[j] ^ key2[j];      
    }
  } else {
    memset(last_block, 0, AES_128_KEY_LENGTH);
    memcpy(last_block, &data[(n*AES_128_KEY_LENGTH)], last);
    for(j = 0; j < AES_128_KEY_LENGTH; j++) {    
      result[j] = last_block[j] ^ e[j] ^ key3[j];      
    }
  }

  aes_128_padded_encrypt(result, AES_128_KEY_LENGTH);
  memcpy(e, result, AES_128_KEY_LENGTH);
  memcpy(mac, e, XCBC_MAC_SIZE);

}

void 
Security_aes_ctr(uint8_t iv[SDDS_SECURITY_IV_SIZE], uint8_t aes_key[AES_128_KEY_LENGTH], uint8_t *data, uint8_t size) {

  int n, last, i, j;
  uint8_t block[AES_128_KEY_LENGTH];
  uint32_t ctr = 0;
  
  n = size / AES_128_KEY_LENGTH;
  last = size % AES_128_KEY_LENGTH;

  aes_128_set_padded_key(aes_key, AES_128_KEY_LENGTH);
  memcpy(block, iv, SDDS_SECURITY_IV_SIZE);

  for(i = 0; i < n; i++) {
    memcpy(block + SDDS_SECURITY_IV_SIZE, &ctr, sizeof(uint32_t));    
    aes_128_padded_encrypt(block, AES_128_KEY_LENGTH);
    for(j = 0; j < AES_128_KEY_LENGTH; j++) {
      data[(i*AES_128_KEY_LENGTH) + j] = data[(i*AES_128_KEY_LENGTH) + j] ^ block[j];
    }
    ctr++;
  }

  if(last) {   
    memcpy(block + SDDS_SECURITY_IV_SIZE, &ctr, sizeof(uint32_t));
    aes_128_padded_encrypt(block, AES_128_KEY_LENGTH);
    for(j = 0; j < last; j++) {    
      data[(n*AES_128_KEY_LENGTH) + j] = data[(n*AES_128_KEY_LENGTH) + j] ^ block[j];
    }
  }

}

bool 
DDS_Security_CryptoTransform_encode_serialized_data(
	OctetSeq *encoded_buffer, /* inout (variable length) */
	OctetSeq *plain_buffer, /* in (variable length) */
	DatawriterCryptoHandle *sending_datawriter_crypto, /* in (fixed length) */
	SecurityException *ex /* inout (variable length) */
) {

  uint8_t iv[SDDS_SECURITY_IV_SIZE];
  uint8_t mac[XCBC_MAC_SIZE];

  if(encoded_buffer->len < (SDDS_SECURITY_IV_SIZE + plain_buffer->len + XCBC_MAC_SIZE)) {
    return false;
  }  

  getRandomBytes(iv, SDDS_SECURITY_IV_SIZE);
  
  Security_aes_ctr(iv, sending_datawriter_crypto->key_material, plain_buffer->data, plain_buffer->len);

  memcpy(encoded_buffer->data, iv, SDDS_SECURITY_IV_SIZE);
  memcpy(encoded_buffer->data + SDDS_SECURITY_IV_SIZE, plain_buffer->data, plain_buffer->len);
  
  Security_aes_xcbc_mac(sending_datawriter_crypto->key_material + AES_128_KEY_LENGTH, 
                        encoded_buffer->data, 
                        SDDS_SECURITY_IV_SIZE + plain_buffer->len, 
                        mac);

  memcpy(encoded_buffer->data + SDDS_SECURITY_IV_SIZE + plain_buffer->len, mac, XCBC_MAC_SIZE);

  printf("encoded buffer: ");
  Security_print_key(encoded_buffer->data, encoded_buffer->len);  

  return true;

}

bool 
DDS_Security_CryptoTransform_decode_serialized_data(
	OctetSeq *plain_buffer, /* inout (variable length) */
	OctetSeq *encoded_buffer, /* in (variable length) */
	DatareaderCryptoHandle *receiving_datareader_crypto, /* in (fixed length) */
	DatawriterCryptoHandle *sending_datawriter_crypto, /* in (fixed length) */
	SecurityException * ex
) {

  uint8_t mac[XCBC_MAC_SIZE];
  uint8_t data_len = encoded_buffer->len - SDDS_SECURITY_IV_SIZE - XCBC_MAC_SIZE - 2;
printf("decoding ....\n");
  if(plain_buffer->len < data_len) {
    return false;
  }  

  printf("encoded: %02x%02x%02x\n", *(encoded_buffer->data + SDDS_SECURITY_IV_SIZE),
*(encoded_buffer->data + SDDS_SECURITY_IV_SIZE + 1), *(encoded_buffer->data + SDDS_SECURITY_IV_SIZE + 2));

  Security_aes_xcbc_mac(receiving_datareader_crypto->key_material + AES_128_KEY_LENGTH, 
                        encoded_buffer->data, 
                        SDDS_SECURITY_IV_SIZE + data_len, 
                        mac);

  if(memcmp(encoded_buffer->data + SDDS_SECURITY_IV_SIZE + data_len, mac, XCBC_MAC_SIZE) != 0) {
    printf("MAC incorrect\n");
    return false;
  }

  Security_aes_ctr(encoded_buffer->data, 
                   receiving_datareader_crypto->key_material, 
                   encoded_buffer->data + SDDS_SECURITY_IV_SIZE, 
                   data_len);

  memcpy(plain_buffer->data, encoded_buffer->data + SDDS_SECURITY_IV_SIZE, data_len);

  return true;

}


void 
Security_get_bytes(uint8_t *res, char* str, int nbytes) {

	int i;
  char buf[3];  
  unsigned chr;

	for(i = 0; i < nbytes; i++) {
    strncpy(buf, &str[2*i], 2);
    buf[2] = '\0';
    chr = (unsigned char) strtol(buf, (char **) NULL, 16);
		res[i] = chr;
	}
	
}

void 
Security_get_string(char *str, uint8_t num[NUM_ECC_DIGITS]) {

  int i;

	for(i = 0; i < NUM_ECC_DIGITS; i++) {
    sprintf(&str[i*2], "%02x", num[i]);
	}

}

void 
Security_print_key(uint8_t *key, int n) {

  int i;

	for(i = 0; i < n; i++) {
    printf("%02x", key[i]);
	}
  printf("\n");

}

#endif
