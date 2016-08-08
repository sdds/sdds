#include <unistd.h>
#include "linux_alpha_sub_sdds_impl.h"
#include "sDDS.h"

int main()
{
	DDS_ReturnCode_t ret;

	if (sDDS_init() == SDDS_RT_FAIL) {
		return 1;
	}
	Log_setLvl(4);  // Disable logs, set to 0 for to see everything.

    static Alpha alpha_sub;
    Alpha *alpha_sub_p = &alpha_sub;

    for (;;) {
        ret = DDS_AlphaDataReader_take_next_sample(g_filteredAlpha1_reader,
                &alpha_sub_p, NULL);
        if (ret == DDS_RETCODE_NO_DATA) {
            printf("no data for alpha (OVERLAPS wohnzimmer)\n");
        }
        else {
            printf("Received a sample from topic 'alpha' (OVERLAPS wohnzimmer): {\n"
                   "   value => %u\n"
                   "   pkey => %u\n"
                   "   value2 => %s\n"
                   "   value3 => %"PRId16"\n"
                   "}\n"
                   , alpha_sub_p->value
                   , alpha_sub_p->pkey
                   , alpha_sub_p->value2
                   , alpha_sub_p->value3);
        }

        ret = DDS_AlphaDataReader_take_next_sample(g_filteredAlpha2_reader,
                &alpha_sub_p, NULL);
        if (ret == DDS_RETCODE_NO_DATA) {
            printf("no data for alpha (WHITHIN schlafzimmer)\n");
        }
        else {
            printf("Received a sample from topic 'alpha' (WHITHIN schlafzimmer): {\n"
                   "   value => %u\n"
                   "   pkey => %u\n"
                   "   value2 => %s\n"
                   "   value3 => %"PRId16"\n"
                   "}\n"
                   , alpha_sub_p->value
                   , alpha_sub_p->pkey
                   , alpha_sub_p->value2
                   , alpha_sub_p->value3);
        }

        sleep (1);
    }

    return 0;
}
