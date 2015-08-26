#include "Debug.h"

void printRC(rc_t ret) {
	switch (ret) {
	case SDDS_RT_OK:
		printf("OK\n");
		break;
	case SDDS_RT_KNOWN:
		printf("KNOWN\n");
		break;
	case SDDS_RT_FAIL:
		printf("FAIL\n");
		break;
	default:
		printf("Unknown value %u\n", ret);
		break;
	}
}
