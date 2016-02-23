#include "sDDS.h"

#ifdef FEATURE_SDDS_SECURITY_ENABLED

rc_t 
Security_init() {

  #ifdef SDDS_SECURITY_KDC
  Security_kdc();    
  #else  
  Security_auth();
  #endif    

  return SDDS_RT_OK;
}

rc_t 
Security_auth() {

  DDS_ReturnCode_t r;  
  DDS_S_Result_t res;
  SecurityException ex;
  HandshakeMessageToken msg_tok[2];
  DDS_ParticipantStatelessMessage msg;

  msg.key = BuiltinTopic_participantID;
  strcpy((char *) msg.message_class_id, GMCLASSID_SECURITY_AUTH_HANDSHAKE);

  //uint8_t shared_secret[NUM_ECC_DIGITS];
  //uint8_t key_material[SDDS_SECURITY_KDF_KEY_BYTES];
  
  res = DDS_Security_Authentication_begin_handshake_request(
    NULL,
    msg_tok, 
    NULL, 
    NULL, 
    NULL, 
    &ex 
  );


  msg.message_data = msg_tok[0];
  r = DDS_ParticipantStatelessMessageDataWriter_write(g_ParticipantStatelessMessage_writer,
                                                    &msg,
                                                    NULL);  
  printf("send cert returned %d\n", r);  

  msg.message_data = msg_tok[1];
  r = DDS_ParticipantStatelessMessageDataWriter_write(g_ParticipantStatelessMessage_writer,
                                                    &msg,
                                                    NULL);  
  printf("send sign and nonce returned %d\n", r);  

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

  return SDDS_RT_OK;

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

  uint8_t nonce[SDDS_SECURITY_KDF_NONCE_SIZE];

  HandshakeMessageToken *msg = handshake_message_out;

  // part 1: certificate
  strcpy(msg[0].class_id, SDDS_SECURITY_CLASS_AUTH_REQ);
  msg[0].props[0] = (Property) {SDDS_SECURITY_PROP_ID, SDDS_SECURITY_USER_ID}; 
  msg[0].props[1] = (Property) {SDDS_SECURITY_PROP_PUB_KEY_X, SDDS_SECURITY_USER_PUBLIC_KEY_X}; 
  msg[0].props[2] = (Property) {SDDS_SECURITY_PROP_PUB_KEY_Y, SDDS_SECURITY_USER_PUBLIC_KEY_Y}; 

  // part 2: signature and nonce
  getRandomBytes(nonce, sizeof(nonce));
  strcpy(msg[1].class_id, SDDS_SECURITY_CLASS_AUTH_REQ);
  msg[1].props[0] = (Property) {SDDS_SECURITY_PROP_SIG_R, SDDS_SECURITY_USER_CERT_SIG_R}; 
  msg[1].props[1] = (Property) {SDDS_SECURITY_PROP_SIG_S, SDDS_SECURITY_USER_CERT_SIG_S}; 
  strcpy(msg[1].props[2].key, SDDS_SECURITY_PROP_NONCE); strncpy(msg[1].props[2].value, nonce, sizeof(nonce));

  return VALIDATION_OK;

}

rc_t 
Security_kdc() {

  DDS_S_Result_t res;
  SecurityException ex;
  
  res = DDS_Security_Authentication_begin_handshake_reply(
    NULL,
    NULL, 
    NULL, 
    NULL, 
    NULL, 
    &ex
  );

  return SDDS_RT_OK;

}

DDS_S_Result_t DDS_Security_Authentication_begin_handshake_reply(
	HandshakeHandle *handshake_handle, /* inout (fixed length) */
	HandshakeMessageToken *handshake_message_out, /* inout (variable length) */
	HandshakeMessageToken *handshake_message_in, /* in (variable length) */
	IdentityHandle *initiator_identity_handle, /* in (fixed length) */
	IdentityHandle *replier_identity_handle, /* in (fixed length) */
	SecurityException *ex /* inout (variable length) */
) {

  DDS_ReturnCode_t r;
  DDS_ParticipantStatelessMessage msg;
  DDS_ParticipantStatelessMessage *ptr = &msg;
  DDS_SampleInfo info;

  puts("ULF");  

  r = DDS_ParticipantStatelessMessageDataReader_take_next_sample(
                                                 g_ParticipantStatelessMessage_reader,
                                                 &ptr,
                                                 &info);

  if (r == DDS_RETCODE_NO_DATA) {
    printf("no data\n");
  } else {
    printf("received sample: \n");
    printf("%15s: %48s\n", msg.message_data.props[0].key, msg.message_data.props[0].value); 
    printf("%15s: %48s\n", msg.message_data.props[1].key, msg.message_data.props[1].value); 
    printf("%15s: %48s\n", msg.message_data.props[2].key, msg.message_data.props[2].value); 
  }

  r = DDS_ParticipantStatelessMessageDataReader_take_next_sample(
                                                 g_ParticipantStatelessMessage_reader,
                                                 &ptr,
                                                 &info);

  if (r == DDS_RETCODE_NO_DATA) {
    printf("no data\n");
  } else {
    printf("received sample: \n");
    printf("%15s: %48s\n", msg.message_data.props[0].key, msg.message_data.props[0].value); 
    printf("%15s: %48s\n", msg.message_data.props[1].key, msg.message_data.props[1].value); 
    printf("%15s: %48s\n", msg.message_data.props[2].key, msg.message_data.props[2].value); 
  }


}

void Security_kdf(uint8_t key_material[SDDS_SECURITY_KDF_KEY_BYTES], uint8_t shared_secret[NUM_ECC_DIGITS], KdfInfo *inf) {

  int reps = (SDDS_SECURITY_KDF_KEY_BYTES / SHA1_HASH_BYTES);
  int r = SDDS_SECURITY_KDF_KEY_BYTES % SHA1_HASH_BYTES;
  if(r) {
    reps++;
  }                                
  char counter_str[32];
  uint8_t hash[SHA1_HASH_BYTES];
  int size = sizeof(counter_str) + NUM_ECC_DIGITS + sizeof(KdfInfo);
  uint8_t data[size];

  for(uint32_t i = 0; i < reps; i++) {
    
    for (int j = 0; j < sizeof(counter_str); j++) {
      counter_str[j] = ((1 << j) & (i + 1)) ? ('1') : ('0');
    }
    
    memcpy(data, counter_str, sizeof(counter_str));
    memcpy(data + sizeof(counter_str), shared_secret, NUM_ECC_DIGITS);
    memcpy(data + sizeof(counter_str) + NUM_ECC_DIGITS, inf, sizeof(KdfInfo));

    sha1(hash, data, size*8);
    
    if(i == reps - 1 && r) {
      memcpy(key_material + (i*SHA1_HASH_BYTES), hash, r);    
    } else {      
      memcpy(key_material + (i*SHA1_HASH_BYTES), hash, SHA1_HASH_BYTES);    
    }

  }
  
}



#endif











