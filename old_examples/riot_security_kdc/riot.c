
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include "riot_sdds_impl.h"
#include "os-ssal/Thread.h"
#include "xtimer.h"
#include "thread.h"

int main(void);

int main(void) {

  DDS_ReturnCode_t ret;  

	if (sDDS_init() == SDDS_RT_FAIL) {
		return 1;
	}
	Log_setLvl(0);

  Security_kdc();

	return 0;

}
