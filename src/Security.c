#include "sDDS.h"

#ifdef FEATURE_SDDS_SECURITY_ENABLED

static MessageIdentity msgid;
static HandshakeHandle g_handle;

rc_t 
Security_init() {
  
  #ifndef SDDS_SECURITY_KDC
  if(Security_auth() == SDDS_RT_FAIL) {
    return SDDS_RT_FAIL;
  }

  /* TODO */

  #endif

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
          printf("sent token\n");
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
          printf("sent final token\n");
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
  handshake_message_out->props[0] = (Property) {SDDS_SECURITY_PROP_ID, SDDS_SECURITY_USER_ID}; 
  handshake_message_out->props[1] = (Property) {SDDS_SECURITY_PROP_PUB_KEY_X, SDDS_SECURITY_USER_PUBLIC_KEY_X}; 
  handshake_message_out->props[2] = (Property) {SDDS_SECURITY_PROP_PUB_KEY_Y, SDDS_SECURITY_USER_PUBLIC_KEY_Y}; 

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
  strncpy(handshake_handle->info.uid, handshake_message_in->props[0].value, CLASS_ID_STRLEN);
  Security_get_bytes(handshake_handle->info.public_key.x, handshake_message_in->props[1].value, sizeof(handshake_handle->info.public_key.x));
  Security_get_bytes(handshake_handle->info.public_key.y, handshake_message_in->props[2].value, sizeof(handshake_handle->info.public_key.y));

  // reply with id and public key in handshake_message_out
  strcpy(handshake_message_out->class_id, SDDS_SECURITY_CLASS_AUTH_REP);
  handshake_message_out->props[0] = (Property) {SDDS_SECURITY_PROP_ID, SDDS_SECURITY_USER_ID}; 
  handshake_message_out->props[1] = (Property) {SDDS_SECURITY_PROP_PUB_KEY_X, SDDS_SECURITY_USER_PUBLIC_KEY_X}; 
  handshake_message_out->props[2] = (Property) {SDDS_SECURITY_PROP_PUB_KEY_Y, SDDS_SECURITY_USER_PUBLIC_KEY_Y}; 

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
      strncpy(handshake_handle->info.uid, handshake_message_in->props[0].value, CLASS_ID_STRLEN);
      Security_get_bytes(handshake_handle->info.public_key.x, handshake_message_in->props[1].value, sizeof(handshake_handle->info.public_key.x));
      Security_get_bytes(handshake_handle->info.public_key.y, handshake_message_in->props[2].value, sizeof(handshake_handle->info.public_key.y));

      // reply with signature and nonce in handshake_message_out
      getRandomBytes(handshake_handle->info.nonce, sizeof(handshake_handle->info.nonce));
      strcpy(handshake_message_out->class_id, SDDS_SECURITY_CLASS_AUTH_REP);
      handshake_message_out->props[0] = (Property) {SDDS_SECURITY_PROP_SIG_R, SDDS_SECURITY_USER_CERT_SIG_R}; 
      handshake_message_out->props[1] = (Property) {SDDS_SECURITY_PROP_SIG_S, SDDS_SECURITY_USER_CERT_SIG_S}; 
      strcpy(handshake_message_out->props[2].key, SDDS_SECURITY_PROP_NONCE); 
      memcpy(handshake_message_out->props[2].value, handshake_handle->info.nonce, sizeof(handshake_handle->info.nonce));

      handshake_handle->state = SDDS_SECURITY_HANDSHAKE_STATE_1;
      res = VALIDATION_PENDING_HANDSHAKE_MESSAGE;
    break;

    case SDDS_SECURITY_HANDSHAKE_STATE_1:

      // fetch remote signature and nonce from handshake_message_in
      Security_get_bytes(handshake_handle->info.signature_r, handshake_message_in->props[0].value, sizeof(handshake_handle->info.signature_r));
      Security_get_bytes(handshake_handle->info.signature_s, handshake_message_in->props[1].value, sizeof(handshake_handle->info.signature_s));
      memcpy(handshake_handle->info.remote_nonce, handshake_message_in->props[2].value, sizeof(handshake_handle->info.remote_nonce));

      // verify user certificate
      if(Security_verify_certificate(handshake_handle) == SDDS_RT_FAIL) {
        res = VALIDATION_FAILED;
        break;
      } else {
        printf("certificate ok\n");
      }

      // calculate shared secret
#ifdef SDDS_SECURITY_KDC
      if(Security_set_key_material(handshake_handle, handshake_handle->info.remote_nonce) == SDDS_RT_FAIL) {
#else
      if(Security_set_key_material(handshake_handle, handshake_handle->info.nonce) == SDDS_RT_FAIL) {
#endif
        res = VALIDATION_FAILED;
        break;
      } else {
        printf("key material ok\n");
      }

#ifdef SDDS_SECURITY_KDC
      // reply with signature and nonce in handshake_message_out
      getRandomBytes(handshake_handle->info.nonce, sizeof(handshake_handle->info.nonce));
      strcpy(handshake_message_out->class_id, SDDS_SECURITY_CLASS_AUTH_REP);
      handshake_message_out->props[0] = (Property) {SDDS_SECURITY_PROP_SIG_R, SDDS_SECURITY_USER_CERT_SIG_R}; 
      handshake_message_out->props[1] = (Property) {SDDS_SECURITY_PROP_SIG_S, SDDS_SECURITY_USER_CERT_SIG_S}; 
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
Security_set_key_material(HandshakeHandle *h, uint8_t nonce[NUM_ECC_DIGITS]) {

  uint8_t private_key[NUM_ECC_DIGITS];
  uint8_t random[NUM_ECC_DIGITS];

  Security_get_bytes(private_key, SDDS_SECURITY_USER_PRIVATE_KEY, NUM_ECC_DIGITS);
  getRandomBytes(random, sizeof(random));

  // calculate shared secret 
  if(!ecdh_shared_secret(h->info.shared_secret, &h->info.public_key, private_key, random)) {
    return SDDS_RT_FAIL;
  }

  // calculate key material (MAC- and AES-Key)
  Security_kdf(h->info.key_material, h->info.shared_secret, nonce);

  return SDDS_RT_OK;

}

rc_t
Security_verify_certificate(HandshakeHandle *h) {

  EccPoint ca_publicKey;
  uint8_t hash[NUM_ECC_DIGITS];
  char str[NUM_ECC_DIGITS * 2 + 1];
	char cert[SDDS_SECURITY_CERT_STRLEN];
  char *p = cert;
  int res;

	snprintf(p, SDDS_SECURITY_USER_ID_STRLEN + 2, 
                "%s,", h->info.uid);
  p += SDDS_SECURITY_USER_ID_STRLEN + 1;

  Security_get_string(str, h->info.public_key.x);
	snprintf(p, NUM_ECC_DIGITS * 2 + 2, "%s,", str);
  p += NUM_ECC_DIGITS * 2 + 1;

  Security_get_string(str, h->info.public_key.y);
	snprintf(p, NUM_ECC_DIGITS * 2 + 1, "%s", str);

	memset(hash, 0, NUM_ECC_DIGITS);
	sha1(hash, cert, 8 * (SDDS_SECURITY_CERT_STRLEN - 1));

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
Security_kdf(uint8_t key_material[SDDS_SECURITY_KDF_KEY_BYTES], 
             uint8_t shared_secret[NUM_ECC_DIGITS], 
             uint8_t nonce[NUM_ECC_DIGITS]) 
{

  int reps = (SDDS_SECURITY_KDF_KEY_BYTES / SHA1_HASH_BYTES);
  int r = SDDS_SECURITY_KDF_KEY_BYTES % SHA1_HASH_BYTES;                           
  char counter[4];  
  uint32_t *ptr = (uint32_t *) counter;
  uint8_t hash[SHA1_HASH_BYTES];
  int size = sizeof(counter) + NUM_ECC_DIGITS * 2;
  uint8_t data[size];

  if(r) {
    reps++;
  }

  for(uint32_t i = 0; i < reps; i++) {
    
    (*ptr)++;
    
    memcpy(data, counter, sizeof(counter));
    memcpy(data + sizeof(counter), shared_secret, NUM_ECC_DIGITS);
    memcpy(data + sizeof(counter) + NUM_ECC_DIGITS, nonce, NUM_ECC_DIGITS);

    sha1(hash, data, size*8);
    
    if(i == reps - 1 && r) {
      memcpy(key_material + (i*SHA1_HASH_BYTES), hash, r);    
    } else {      
      memcpy(key_material + (i*SHA1_HASH_BYTES), hash, SHA1_HASH_BYTES);    
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

  for(i = 0; i < n-1; i++) {
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

}

#endif
