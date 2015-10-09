#include "riot_sdds_impl.h"
#include "alpha-ds.h"
#include "Log.h"
#include "vtimer.h"

int main() {
	sDDS_init();
	Log_setLvl(0);

	DDS_ReturnCode_t ret;

	Alpha alpha_data_used;
	Alpha* alpha_data_used_ptr = &alpha_data_used;

	Beta beta_data_used;
	Beta* beta_data_used_ptr = &beta_data_used;

	while (1) {

		do {
			ret = DDS_AlphaDataReader_take_next_sample(g_Alpha_reader,
					&alpha_data_used_ptr, NULL);
			if (ret == DDS_RETCODE_NO_DATA) {
				printf("no data alpha\n");
			} else {
				printf("Received (alpha): %d\n", alpha_data_used.value);
			}
		} while (ret != DDS_RETCODE_NO_DATA);

		do {
			ret = DDS_BetaDataReader_take_next_sample(g_Beta_reader,
					&beta_data_used_ptr, NULL);
			if (ret == DDS_RETCODE_NO_DATA) {
				printf("no data beta\n");
			} else {
				printf("Received (beta): %d\n", beta_data_used.value);
			}
		} while (ret != DDS_RETCODE_NO_DATA);

		vtimer_usleep(10 *SEC_IN_USEC);
	}

}
