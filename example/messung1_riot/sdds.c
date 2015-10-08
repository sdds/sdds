#include "riot_sdds_impl.h"
#include "alpha-ds.h"
#include "Log.h"
#include "vtimer.h"

int main() {
	sDDS_init();
	Log_setLvl(0);

	DDS_ReturnCode_t ret;

	Alpha a;
	a.value = 0;

	for (;;) {
		if (DDS_AlphaDataWriter_write(g_Alpha_writer, &a,
				NULL) != DDS_RETCODE_OK) {
			// handle error
		}
		a.value++;
		vtimer_usleep(10 *SEC_IN_USEC);
	}

}
