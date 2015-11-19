#include "messung6_linux_sdds_impl.h"
#include <stdio.h>
#include <unistd.h>

int main()
{
	DDS_ReturnCode_t ret;

	sDDS_init();
	Log_setLvl(0);

    Alpha alpha_pub;
    Beta beta_pub;
    Gamma gamma_sub;
    Gamma *gamma_sub_p = &gamma_sub;
    Delta delta_sub;
    Delta *delta_sub_p = &delta_sub;

    for (;;) {
        ret = DDS_AlphaDataWriter_write (g_Alpha_writer, &alpha_pub, NULL);
        if (ret != DDS_RETCODE_OK)
            printf ("Failed to send topic alpha\n");

        ret = DDS_BetaDataWriter_write (g_Beta_writer, &beta_pub, NULL);
        if (ret != DDS_RETCODE_OK)
            printf ("Failed to send topic beta\n");

        ret = DDS_GammaDataReader_take_next_sample(g_Gamma_reader,
                &gamma_sub_p, NULL);
        if (ret == DDS_RETCODE_NO_DATA) {
            printf("no data for gamma\n");
        }
        else {
            printf("Received (gamma)\n");
        }

        ret = DDS_DeltaDataReader_take_next_sample(g_Delta_reader,
                &delta_sub_p, NULL);
        if (ret == DDS_RETCODE_NO_DATA) {
            printf("no data for delta\n");
        }
        else {
            printf("Received (delta)\n");
        }

        sleep (1);
    }

    return 0;
}
