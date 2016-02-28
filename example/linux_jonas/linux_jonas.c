#include <unistd.h>
#include "linux_jonas_sdds_impl.h"

int main(void) {

  DDS_ReturnCode_t ret;  

  Beta beta_sub;
  Beta *beta_sub_p = &beta_sub;

	if (sDDS_init() == SDDS_RT_FAIL) {
		return 1;
	}
	Log_setLvl(0);

  for (;;) {

    Security_init();
    sleep(5);

  }

	return 0;
}
