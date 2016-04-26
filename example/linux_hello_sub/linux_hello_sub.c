#include <unistd.h>
#include "linux_hello_sub_sdds_impl.h"

int main()
{
	DDS_ReturnCode_t ret;

	if (sDDS_init() == SDDS_RT_FAIL) {
		return 1;
	}
	Log_setLvl(6);  // Disable logs, set to 0 for to see everything.

    static Hello hello_sub;
    Hello *hello_sub_p = &hello_sub;

    for (;;) {
        ret = DDS_HelloDataReader_take_next_sample(g_Hello_reader,
                &hello_sub_p, NULL);
        if (ret == DDS_RETCODE_NO_DATA) {
            printf("no data for hello\n");
        }
        else {
            printf("Received a sample from topic 'hello': {\n"
                   "   value => %s\n"
                   "}\n"
                   , hello_sub_p->value);
        }

        sleep (1);
    }

    return 0;
}
