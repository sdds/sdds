#include <unistd.h>
#include "dings_sdds_impl.h"

int main()
{
	DDS_ReturnCode_t ret;

	if (sDDS_init() == SDDS_RT_FAIL) {
		return 1;
	}
	Log_setLvl(6);  // Disable logs, set to 0 for to see everything.

    static Alpha alpha_pub;
    alpha_pub.value = 'H';
    static Alpha alpha_sub;
    Alpha *alpha_sub_p = &alpha_sub;


    sleep (1);
    printf("\n");
    printf ("##########################################################################################\n");
    printf ("#                                  HISTORY DEMO                                           \n");
    printf ("##########################################################################################\n");

    printf ("Waiting for discovery to complete ...\n");
    sleep (10);
    sdds_History_print (DataReader_history (g_Alpha_reader));
    for (;;) {
        printf ("=> ENQUEUE\n");
        ret = DDS_AlphaDataWriter_write (g_Alpha_writer, &alpha_pub, NULL);

        sleep(1);
        sdds_History_print (DataReader_history (g_Alpha_reader));
        getchar();

        printf ("=> DEQUEUE\n");
        ret = DDS_AlphaDataReader_take_next_sample(g_Alpha_reader, &alpha_sub_p, NULL);

        sleep(1);
        sdds_History_print (DataReader_history (g_Alpha_reader));
        getchar();

        printf ("=> ENQUEUE x 4\n");
        ret = DDS_AlphaDataWriter_write (g_Alpha_writer, &alpha_pub, NULL); usleep (250);
        ret = DDS_AlphaDataWriter_write (g_Alpha_writer, &alpha_pub, NULL); usleep (250);
        ret = DDS_AlphaDataWriter_write (g_Alpha_writer, &alpha_pub, NULL); usleep (250);
        ret = DDS_AlphaDataWriter_write (g_Alpha_writer, &alpha_pub, NULL); usleep (250);

        sleep(2);
        sdds_History_print (DataReader_history (g_Alpha_reader));
        getchar();

        printf ("=> ENQUEUE\n");
        ret = DDS_AlphaDataWriter_write (g_Alpha_writer, &alpha_pub, NULL);

        sleep(1);
        sdds_History_print (DataReader_history (g_Alpha_reader));
        getchar();

        printf ("=> ENQUEUE\n");
        ret = DDS_AlphaDataWriter_write (g_Alpha_writer, &alpha_pub, NULL);

        sleep(1);
        sdds_History_print (DataReader_history (g_Alpha_reader));
        getchar();

        printf ("=> ENQUEUE\n");
        ret = DDS_AlphaDataWriter_write (g_Alpha_writer, &alpha_pub, NULL);

        sleep(1);
        sdds_History_print (DataReader_history (g_Alpha_reader));
        getchar();

        printf ("=> DEQUEUE x 3\n");
        ret = DDS_AlphaDataReader_take_next_sample(g_Alpha_reader, &alpha_sub_p, NULL);
        ret = DDS_AlphaDataReader_take_next_sample(g_Alpha_reader, &alpha_sub_p, NULL);
        ret = DDS_AlphaDataReader_take_next_sample(g_Alpha_reader, &alpha_sub_p, NULL);

        sleep(1);
        sdds_History_print (DataReader_history (g_Alpha_reader));
        getchar();

        printf ("=> DEQUEUE\n");
        ret = DDS_AlphaDataReader_take_next_sample(g_Alpha_reader, &alpha_sub_p, NULL);

        sleep(1);
        sdds_History_print (DataReader_history (g_Alpha_reader));
        getchar();
    }

    return 0;
}
