#include "sDDS.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void getRandomBytes(uint8_t *p_dest, unsigned p_size) {

	int randfd = open("/dev/urandom", O_RDONLY);
	if(randfd == -1) {
		printf("No access to urandom\n");
	}

	if(read(randfd, p_dest, p_size) != (int)p_size) {
		printf("Failed to get random bytes.\n");
	}

}
