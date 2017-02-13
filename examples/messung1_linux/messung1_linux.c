#include "messung1_linux_sdds_impl.h"
#include "alpha-ds.h"
#include "Log.h"
#include <unistd.h>

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
		sleep(10);
	}

}
