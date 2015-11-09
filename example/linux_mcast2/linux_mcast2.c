#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "linux_mcast2_sdds_impl.h"

int main() {
	pthread_t t;

	sDDS_init();
	Log_setLvl(0);

	DDS_ReturnCode_t ret;

	Beta beta_data_used;
	Beta* beta_data_used_ptr = &beta_data_used;

	for (;;) {

		do {
			ret = DDS_BetaDataReader_take_next_sample(g_Beta_reader,
					&beta_data_used_ptr, NULL);
			if (ret == DDS_RETCODE_NO_DATA) {
				printf("no data beta\n");
			} else {
				printf("Received (beta):[%u] %s %s\n", beta_data_used.value,
						beta_data_used.value2, beta_data_used.value3);
			}
		} while (ret != DDS_RETCODE_NO_DATA);

		sleep(10);

	}
}
