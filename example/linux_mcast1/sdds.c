#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "linux_sdds_impl.h"
#include "beta-ds.h"
#include "BuiltinTopic.h"
#include "Log.h"
#include "DataSource.h"
#include "DataSink.h"
#include "Discovery.h"

#include "os-ssal/NodeConfig.h"

int main() {
	sDDS_init();
	Log_setLvl(0);

	DDS_ReturnCode_t ret;

	Beta beta_data_used;
	Beta* beta_data_used_ptr = &beta_data_used;

	Beta b;
	b.value = 44;
	memset(b.value2, 0, 10);
	memset(b.value3, 0, 9);
	b.value2[0] = 'D';
	b.value2[1] = 'e';
	b.value2[2] = 'r';
	b.value2[3] = '\0';
	b.value3[0] = 'B';
	b.value3[1] = 'a';
	b.value3[2] = 'u';
	b.value3[3] = 'm';
	b.value3[4] = '!';
	b.value3[5] = '\0';

	uint32_t b_val = 0;

	for (;;) {
		b.value = b_val++;
		if (DDS_BetaDataWriter_write(g_Beta_writer, &b,
				NULL) != DDS_RETCODE_OK) {
			// handle error
		}

		sleep(10);
	}

}
