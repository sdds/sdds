
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include "riot_sdds_impl.h"
#include "ipc-ds.h"
#include "os-ssal/Thread.h"
#include "xtimer.h"
#include "thread.h"

int main(void);

int main(void)
{
	Log_setLvl(0);

	sDDS_init();
	printf("Initilaized sDDS\n");

	for (;;)
	{
		xtimer_usleep(2 *SEC_IN_USEC);
	}
}
