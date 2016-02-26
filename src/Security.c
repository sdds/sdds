#include "sDDS.h"

#ifdef FEATURE_SDDS_SECURITY_ENABLED

#ifdef SDDS_SECURITY_KDC
HandshakeHandle g_handles[SDDS_SECURITY_HANDSHAKE_MAX];
#else  
HandshakeHandle g_handle = {SDDS_SECURITY_HANDSHAKE_STATE_CERT, { 0 }};
#endif    

rc_t 
Security_init() {

  DDS_ReturnCode_t r;  

#ifdef SDDS_SECURITY_KDC
  r = Security_kdc();    
#else  
  r = Security_auth();
#endif    

  return r;
}

#ifndef SDDS_SECURITY_KDC 
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
  int j;

  while(1) {
    
    if(g_handle.state == SDDS_SECURITY_HANDSHAKE_STATE_CERT) {
      res = DDS_Security_Authentication_begin_handshake_request(
        &g_handle,
        &msg_tok_out, 
        &msg_tok_in, 
        NULL, 
        NULL, 
        &ex 
      );
    }

    switch(res) {

      case VALIDATION_OK:
        return SDDS_RT_OK;
      break;

      case VALIDATION_FAILED:
        return SDDS_RT_FAIL;
      break;

      case VALIDATION_PENDING_HANDSHAKE_MESSAGE:      
        // send token
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
            res = process_handshake(
	            &msg_tok_out,
	            &msg_tok_in,
	            &g_handle,
	            &ex
            );
            break;
          }
                         
        }
      break;

      case VALIDATION_OK_FINAL_MESSAGE:
        // send final token
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
        }
      break;

    }

  }
    
  return SDDS_RT_OK;

}

#else 

rc_t 
Security_kdc() {

  DDS_S_Result_t res = VALIDATION_FAILED;
  DDS_ReturnCode_t r;  
  SecurityException ex;
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

    if(r == DDS_RETCODE_OK) {
      printf("received message\n");
      printf("string: %s\n", msg.message_data.class_id);
      printf("%d\n", strcmp(msg.message_data.class_id, SDDS_SECURITY_CLASS_AUTH_REQ));
      if(strcmp(msg.message_data.class_id, SDDS_SECURITY_CLASS_AUTH_REQ) == 0) {

        printf("its a request\n");
      }
      msg_tok_in = msg.message_data;
    }

  }


  return SDDS_RT_OK;

}
#endif

DDS_S_Result_t 
DDS_Security_Authentication_begin_handshake_request(
	HandshakeHandle *handshake_handle, /* inout (fixed length) */
	HandshakeMessageToken *handshake_message_out, /* in (variable length) */
	HandshakeMessageToken *handshake_message_in, /* in (variable length) */
	IdentityHandle *initiator_identity_handle, /* in (fixed length) */
	IdentityHandle *replier_identity_handle, /* in (fixed length) */
	SecurityException *ex /* inout (variable length) */
) {

  // start with certificate
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

  // fetch remote id and public key from handshake_message_in
  strncpy(handshake_handle->info.uid, handshake_message_in->props[0].value, CLASS_ID_STRLEN);
  Security_get_bytes(handshake_handle->info.public_key.x, handshake_message_in->props[1].value);
  Security_get_bytes(handshake_handle->info.public_key.y, handshake_message_in->props[2].value);

  // start with certificate
  strcpy(handshake_message_out->class_id, SDDS_SECURITY_CLASS_AUTH_REP);
  handshake_message_out->props[0] = (Property) {SDDS_SECURITY_PROP_ID, SDDS_SECURITY_USER_ID}; 
  handshake_message_out->props[1] = (Property) {SDDS_SECURITY_PROP_PUB_KEY_X, SDDS_SECURITY_USER_PUBLIC_KEY_X}; 
  handshake_message_out->props[2] = (Property) {SDDS_SECURITY_PROP_PUB_KEY_Y, SDDS_SECURITY_USER_PUBLIC_KEY_Y}; 

  return VALIDATION_PENDING_HANDSHAKE_MESSAGE;

}

DDS_S_Result_t 
process_handshake(
	HandshakeMessageToken *handshake_message_out,
	HandshakeMessageToken *handshake_message_in,
	HandshakeHandle *handshake_handle,
	SecurityException *ex
) {
  
  DDS_S_Result_t res = VALIDATION_FAILED;

  switch (handshake_handle->state) {

    case SDDS_SECURITY_HANDSHAKE_STATE_CERT:
      res = VALIDATION_PENDING_HANDSHAKE_MESSAGE;
#ifdef SDDS_SECURITY_KDC
      // fetch remote signature and nonce from handshake_message_in
      Security_get_bytes(handshake_handle->info.signature_r, handshake_message_in->props[0].value);
      Security_get_bytes(handshake_handle->info.signature_s, handshake_message_in->props[1].value);
      Security_get_bytes(handshake_handle->info.remote_nonce, handshake_message_in->props[2].value);
#else
      // fetch remote id and public key from handshake_message_in
      strncpy(handshake_handle->info.uid, handshake_message_in->props[0].value, CLASS_ID_STRLEN);
      Security_get_bytes(handshake_handle->info.public_key.x, handshake_message_in->props[1].value);
      Security_get_bytes(handshake_handle->info.public_key.y, handshake_message_in->props[2].value);
#endif
      // set up token to send (signature and nonce) to handshake_message_out
      getRandomBytes(handshake_handle->info.nonce, sizeof(handshake_handle->info.nonce));
      strcpy(handshake_message_out->class_id, SDDS_SECURITY_CLASS_AUTH_REP);
      handshake_message_out->props[0] = (Property) {SDDS_SECURITY_PROP_SIG_R, SDDS_SECURITY_USER_CERT_SIG_R}; 
      handshake_message_out->props[1] = (Property) {SDDS_SECURITY_PROP_SIG_S, SDDS_SECURITY_USER_CERT_SIG_S}; 
      strcpy(handshake_message_out->props[2].key, SDDS_SECURITY_PROP_NONCE); 
      strncpy(handshake_message_out->props[2].value, handshake_handle->info.nonce, sizeof(handshake_handle->info.nonce));

      handshake_handle->state = SDDS_SECURITY_HANDSHAKE_STATE_SIGN;
    break;

    case SDDS_SECURITY_HANDSHAKE_STATE_SIGN:
      res = VALIDATION_PENDING_HANDSHAKE_MESSAGE;
#ifdef SDDS_SECURITY_KDC
      // fetch remote mactag from handshake_message_in
      Security_get_bytes(handshake_handle->info.mactag, handshake_message_in->props[0].value);
#else
      // fetch remote signature and nonce from handshake_message_in
      Security_get_bytes(handshake_handle->info.signature_r, handshake_message_in->props[0].value);
      Security_get_bytes(handshake_handle->info.signature_s, handshake_message_in->props[1].value);
      Security_get_bytes(handshake_handle->info.remote_nonce, handshake_message_in->props[2].value);
#endif
      // set up token to send (mactag) to handshake_message_out
      strcpy(handshake_message_out->class_id, SDDS_SECURITY_CLASS_AUTH_REP);
      handshake_message_out->props[0] = (Property) {SDDS_SECURITY_PROP_MACTAG, "bla"}; 

      handshake_handle->state = SDDS_SECURITY_HANDSHAKE_STATE_MACT;
    break;

    case SDDS_SECURITY_HANDSHAKE_STATE_MACT:
#ifdef SDDS_SECURITY_KDC
      // set up token to send (mactag) to handshake_message_out
      strcpy(handshake_message_out->class_id, SDDS_SECURITY_CLASS_AUTH_REP);
      handshake_message_out->props[0] = (Property) {SDDS_SECURITY_PROP_MACTAG, "bla"}; 
      res = VALIDATION_OK_FINAL_MESSAGE;
#else
      Security_get_bytes(handshake_handle->info.mactag, handshake_message_in->props[0].value);
      res = VALIDATION_OK;
#endif
    break;

  }

  return res;

}






void Security_get_bytes(uint8_t res[NUM_ECC_DIGITS], char* str) {

	int i;
	uint8_t bytes[NUM_ECC_DIGITS];
  char buf[3];  
  unsigned chr;

	for(i = 0; i < NUM_ECC_DIGITS; i++) {
    strncpy(buf, &str[2*i], 2);
    buf[2] = '\0';
    chr = (unsigned char) strtol(buf, (char **) NULL, 16);
		bytes[i] = chr;
	}

	ecc_bytes2native(res, bytes);
	
}

void Security_kdf(uint8_t key_material[SDDS_SECURITY_KDF_KEY_BYTES], uint8_t shared_secret[NUM_ECC_DIGITS], uint8_t nonce[NUM_ECC_DIGITS]) {

  int reps = (SDDS_SECURITY_KDF_KEY_BYTES / SHA1_HASH_BYTES);
  int r = SDDS_SECURITY_KDF_KEY_BYTES % SHA1_HASH_BYTES;
  if(r) {
    reps++;
  }                                
  char counter_str[32];
  uint8_t hash[SHA1_HASH_BYTES];
  int size = sizeof(counter_str) + NUM_ECC_DIGITS + sizeof(nonce);
  uint8_t data[size];

  for(uint32_t i = 0; i < reps; i++) {
    
    for (int j = 0; j < sizeof(counter_str); j++) {
      counter_str[j] = ((1 << j) & (i + 1)) ? ('1') : ('0');
    }
    
    memcpy(data, counter_str, sizeof(counter_str));
    memcpy(data + sizeof(counter_str), shared_secret, NUM_ECC_DIGITS);
    memcpy(data + sizeof(counter_str) + NUM_ECC_DIGITS, nonce, sizeof(nonce));

    sha1(hash, data, size*8);
    
    if(i == reps - 1 && r) {
      memcpy(key_material + (i*SHA1_HASH_BYTES), hash, r);    
    } else {      
      memcpy(key_material + (i*SHA1_HASH_BYTES), hash, SHA1_HASH_BYTES);    
    }

  }
  
}






/* 
  Security_kdf(key_material, shared_secret, &inf);

  printf("kdf derived key material: ");
  printf("size %d\n", sizeof(key_material));
  for(int i = 0; i < sizeof(key_material); i+=2) {
    printf("%x%x", key_material[i], key_material[i+1]);
  }
  printf("\n");

  Security_kdf(key_material, shared_secret, &inf);

  printf("kdf derived key material: ");
  printf("size %d\n", sizeof(key_material));
  for(int i = 0; i < sizeof(key_material); i+=2) {
    printf("%x%x", key_material[i], key_material[i+1]);
  }
  printf("\n");
*/

#endif
