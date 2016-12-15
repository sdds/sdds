#include "sDDS.h"

void getRandomBytes(uint8_t *p_dest, unsigned p_size) {

	int i;
	for(i = 0; i < p_size; i++) {
		p_dest[i] = (uint8_t) random_rand();
	}

}
