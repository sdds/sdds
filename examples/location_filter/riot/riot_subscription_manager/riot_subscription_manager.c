#include <unistd.h>
#include "riot_subscription_manager_sdds_impl.h"

int main()
{
	DDS_ReturnCode_t ret;

	Log_setLvl(3);  // Disable logs, set to 0 for to see everything.
	if (sDDS_init() == SDDS_RT_FAIL) {
		return 1;
	}

    for (;;) {
        sleep (10);
    }

    return 0;
}
