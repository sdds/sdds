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

	Gamma gamma_data_used;
	Gamma* gamma_data_used_ptr = &gamma_data_used;


	Delta delta_data_used;
	Delta* delta_data_used_ptr = &delta_data_used;


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

		do {
			ret = DDS_GammaDataReader_take_next_sample(g_Gamma_reader,
					&gamma_data_used_ptr, NULL);
			if (ret == DDS_RETCODE_NO_DATA) {
				printf("no data gamma\n");
			} else {
				printf("Received (gamma): %d\n", gamma_data_used.value);
			}
		} while (ret != DDS_RETCODE_NO_DATA);

		do {
			ret = DDS_DeltaDataReader_take_next_sample(g_Delta_reader,
					&delta_data_used_ptr, NULL);
			if (ret == DDS_RETCODE_NO_DATA) {
				printf("no data delta\n");
			} else {
				printf("Received (delta): %d\n", delta_data_used.value);
			}
		} while (ret != DDS_RETCODE_NO_DATA);

		sleep(10);
	}

}
