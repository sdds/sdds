#include <unistd.h>
#include "linux_subscription_manager_sdds_impl.h"

int main()
{
	DDS_ReturnCode_t ret;

	if (sDDS_init() == SDDS_RT_FAIL) {
		return 1;
	}
	Log_setLvl(5);  // Disable logs, set to 0 for to see everything.

    for (;;) {
        sleep (1);
    }

    return 0;
}
