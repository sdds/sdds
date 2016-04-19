#include <unistd.h>
#include "linux_jonas_sdds_impl.h"

int main(void) {

  DDS_ReturnCode_t ret;  

	if (sDDS_init() == SDDS_RT_FAIL) {
		return 1;
	}
	Log_setLvl(0);

  Security_kdc();

	return 0;
}
