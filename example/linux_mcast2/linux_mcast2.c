#include "linux_mcast2_sdds_impl.h"
#include "unistd.h"

int main()
{
	DDS_ReturnCode_t ret;

	sDDS_init();
	Log_setLvl(0);

    Beta beta_sub;
    Beta *beta_sub_p = &beta_sub;

    for (;;) {
        ret = DDS_BetaDataReader_take_next_sample(g_Beta_reader,
                &beta_sub_p, NULL);
        if (ret == DDS_RETCODE_NO_DATA) {
            printf("no data for beta\n");
        }
        else {
            printf("Received a sample from topic 'beta': {\n"
                   "   value => %c\n"
                   "   value2 => %s\n"
                   "   value3 => %s\n"
                   "}\n"
                   , beta_sub_p->value
                   , beta_sub_p->value2
                   , beta_sub_p->value3);
        }

        sleep (1);
    }

    return 0;
}
