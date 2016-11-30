#include "sDDS.h"

bool init = 0;

void getRandomBytes(uint8_t *p_dest, unsigned p_size) {

  if(!init) {
    /* replace with value from TRNG !!! */
    random_init(0);
    init = 1;
  }

	int i;
	for(i = 0; i < p_size; i++) {
		p_dest[i] = (uint8_t) random_uint32();
	}

}
