#ifndef __SECURITY_H__
#define __SECURITY_H__

#include "Sha1.h"
#include "Ecc.h"

#define DDS_S_Result_t int
#define VALIDATION_OK	0
#define VALIDATION_FAILED	1
#define VALIDATION_PENDING_RETRY	2
#define VALIDATION_PENDING_HANDSHAKE_REQUEST	3
#define VALIDATION_PENDING_HANDSHAKE_MESSAGE	4
#define VALIDATION_OK_FINAL_MESSAGE	5

#define GMCLASSID_SECURITY_AUTH_HANDSHAKE "dds.sec.auth"

#define SDDS_SECURITY_CLASS_AUTH_REQ "DDS:Auth:Req"

#define SDDS_SECURITY_PROP_ID "dds.sec.id"
#define SDDS_SECURITY_PROP_PUB_KEY_X "dds.sec.x"
#define SDDS_SECURITY_PROP_PUB_KEY_Y "dds.sec.y"
#define SDDS_SECURITY_PROP_SIG_R "dds.sec.r"
#define SDDS_SECURITY_PROP_SIG_S "dds.sec.s"
#define SDDS_SECURITY_PROP_NONCE "nonce"

#define SDDS_SECURITY_KDF_NONCE_SIZE 20
#define SDDS_SECURITY_KDF_KEY_BYTES 19

typedef struct {
	char* message;
	long code;
	long minor_code;
} SecurityException;

typedef DataHolder Token;
typedef Token HandshakeMessageToken;
typedef void HandshakeHandle;
typedef void IdentityHandle;
typedef void SharedSecretHandle;

typedef struct {
  char algId;
  uint8_t nonceInfo[SDDS_SECURITY_KDF_NONCE_SIZE];
} KdfInfo;

rc_t 
Security_init();

rc_t 
Security_auth();

rc_t 
Security_kdc();

void 
Security_kdf(uint8_t key_material[SDDS_SECURITY_KDF_KEY_BYTES], uint8_t shared_secret[NUM_ECC_DIGITS], KdfInfo *inf);

DDS_S_Result_t 
DDS_Security_Authentication_begin_handshake_request(
	HandshakeHandle *handshake_handle, /* inout (fixed length) */
	HandshakeMessageToken *handshake_message_out, /* in (variable length) */
	HandshakeMessageToken *handshake_message_in, /* in (variable length) */
	IdentityHandle *initiator_identity_handle, /* in (fixed length) */
	IdentityHandle *replier_identity_handle, /* in (fixed length) */
	SecurityException *ex /* inout (variable length) */
);

DDS_S_Result_t 
DDS_Security_Authentication_begin_handshake_reply(
	HandshakeHandle *handshake_handle, /* inout (fixed length) */
	HandshakeMessageToken *handshake_message_out, /* inout (variable length) */
	HandshakeMessageToken *handshake_message_in, /* in (variable length) */
	IdentityHandle *initiator_identity_handle, /* in (fixed length) */
	IdentityHandle *replier_identity_handle, /* in (fixed length) */
	SecurityException *ex /* inout (variable length) */
);

DDS_S_Result_t 
process_handshake(
	HandshakeMessageToken *handshake_message_out,
	HandshakeMessageToken *handshake_message_in,
	HandshakeHandle *handshake_handle,
	SecurityException *ex
);

SharedSecretHandle 
get_shared_secret(
	HandshakeHandle *handshake_handle,
	SecurityException *ex
);


#endif  /* __SECURITY_H__ */
