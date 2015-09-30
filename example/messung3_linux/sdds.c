#include "linux_sdds_impl.h"
#include "alpha-ds.h"
#include "Log.h"

int main() {
	sDDS_init();
	Log_setLvl(0);

	DDS_ReturnCode_t ret;

	Alpha alpha_data_used;
	Alpha* alpha_data_used_ptr = &alpha_data_used;

	Beta b;
	b.value = 0;

	for (;;) {

		do {
			ret = DDS_AlphaDataReader_take_next_sample(g_Alpha_reader,
					&alpha_data_used_ptr, NULL);
			if (ret == DDS_RETCODE_NO_DATA) {
				printf("no data alpha\n");
			} else {
				printf("Received (alpha): %d\n", alpha_data_used.value);
			}
		} while (ret != DDS_RETCODE_NO_DATA);

		if (DDS_BetaDataWriter_write(g_Beta_writer, &b,
				NULL) != DDS_RETCODE_OK) {
			// handle error
		}
		b.value++;

		sleep(10);
	}

}
