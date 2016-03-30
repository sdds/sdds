#include <unistd.h>
#include "linux_jonas2_sdds_impl.h"

int main() {
	DDS_ReturnCode_t ret;

	if (sDDS_init() == SDDS_RT_FAIL) {
		return 1;
	}

	Log_setLvl(0);

  for (;;) {
    Security_receive_key();
    sleep (1);
  }

  return 0;
}
