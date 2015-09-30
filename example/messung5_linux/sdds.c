#include "linux_sdds_impl.h"
#include "alpha-ds.h"
#include "Log.h"

int main() {
	sDDS_init();
	Log_setLvl(0);

	DDS_ReturnCode_t ret;

	Alpha a;
	a.value = 0;

	Beta b;
	b.value = 0;

	while (1) {

		if (DDS_AlphaDataWriter_write(g_Alpha_writer, &a,
				NULL) != DDS_RETCODE_OK) {
			// handle error
		}
		a.value++;

		if (DDS_BetaDataWriter_write(g_Beta_writer, &b,
				NULL) != DDS_RETCODE_OK) {
			// handle error
		}
		b.value++;

		sleep(10);
	}

}
