#include <unistd.h>
#include "linux_jonas3_sdds_impl.h"

int main() {
	DDS_ReturnCode_t ret;
  Beta beta_pub;

	if (sDDS_init() == SDDS_RT_FAIL) {
		return 1;
	}

	Log_setLvl(0);

  sleep(1);
  Security_receive_key();

  beta_pub.value = 'H';  
  strncpy(beta_pub.value2,   "Es gibt im", 10);
  strncpy(beta_pub.value3,   "Es gibt i", 9);

  for (;;) {
    ret = DDS_BetaDataWriter_write(g_Beta_writer, &beta_pub, NULL);
    sleep (5);
  }

  return 0;
}
