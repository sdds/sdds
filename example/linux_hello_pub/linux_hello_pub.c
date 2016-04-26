#include <unistd.h>
#include "linux_hello_pub_sdds_impl.h"

int main()
{
	DDS_ReturnCode_t ret;

	if (sDDS_init() == SDDS_RT_FAIL) {
		return 1;
	}
	Log_setLvl(6);  // Disable logs, set to 0 for to see everything.

    static Hello hello_pub;
    strncpy(hello_pub.value,   "Hello World", 11);

    for (;;) {
        ret = DDS_HelloDataWriter_write (g_Hello_writer, &hello_pub, NULL);
        if (ret != DDS_RETCODE_OK) {
            printf ("Failed to send topic hello\n");
        }
        else {
            printf ("Send a hello sample\n");
        }

        sleep (1);
    }

    return 0;
}
