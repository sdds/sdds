#include <unistd.h>
#include "bums_sdds_impl.h"

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
    printf ("#                              HISTORY RELIABILITY DEMO                                  #\n");
    printf ("##########################################################################################\n");

    printf ("Waiting for discovery to complete ...\n");
    sleep (7);
    printf ("Ready!\n");
    sdds_History_print (DataReader_history (g_Alpha_reader));
    for (;;) {
        getchar();
        printf ("=> ENQUEUE\n");
        ret = DDS_AlphaDataWriter_write (g_Alpha_writer, &alpha_pub, NULL);

        usleep(250);
        sdds_History_print (DataReader_history (g_Alpha_reader));
        getchar();

        printf ("=> DEQUEUE\n");
        ret = DDS_AlphaDataReader_take_next_sample(g_Alpha_reader, &alpha_sub_p, NULL);

        usleep(250);
        sdds_History_print (DataReader_history (g_Alpha_reader));
        getchar();

        printf ("=> ENQUEUE x 4\n");
        ret = DDS_AlphaDataWriter_write (g_Alpha_writer, &alpha_pub, NULL); usleep (250);
        ret = DDS_AlphaDataWriter_write (g_Alpha_writer, &alpha_pub, NULL); usleep (250);
        ret = DDS_AlphaDataWriter_write (g_Alpha_writer, &alpha_pub, NULL); usleep (250);
        ret = DDS_AlphaDataWriter_write (g_Alpha_writer, &alpha_pub, NULL); usleep (250);

        sleep(1);
        sdds_History_print (DataReader_history (g_Alpha_reader));
        getchar();

        printf ("=> ENQUEUE\n");
        ret = DDS_AlphaDataWriter_write (g_Alpha_writer, &alpha_pub, NULL);

        usleep(250);
        sdds_History_print (DataReader_history (g_Alpha_reader));
        getchar();

        printf ("=> ENQUEUE\n");
        ret = DDS_AlphaDataWriter_write (g_Alpha_writer, &alpha_pub, NULL);

        usleep(250);
        sdds_History_print (DataReader_history (g_Alpha_reader));
        getchar();

        printf ("=> ENQUEUE\n");
        ret = DDS_AlphaDataWriter_write (g_Alpha_writer, &alpha_pub, NULL);

        usleep(250);
        sdds_History_print (DataReader_history (g_Alpha_reader));
        getchar();
        sdds_History_print (DataReader_history (g_Alpha_reader));
        getchar();

        printf ("=> ENQUEUE\n");
        ret = DDS_AlphaDataWriter_write (g_Alpha_writer, &alpha_pub, NULL);

        usleep(250);
        sdds_History_print (DataReader_history (g_Alpha_reader));
        getchar();

        printf ("=> DEQUEUE\n");
        ret = DDS_AlphaDataReader_take_next_sample(g_Alpha_reader, &alpha_sub_p, NULL);

        usleep(250);
        sdds_History_print (DataReader_history (g_Alpha_reader));
        getchar();

        printf ("=> DEQUEUE x 3\n");
        ret = DDS_AlphaDataReader_take_next_sample(g_Alpha_reader, &alpha_sub_p, NULL);
        ret = DDS_AlphaDataReader_take_next_sample(g_Alpha_reader, &alpha_sub_p, NULL);
        ret = DDS_AlphaDataReader_take_next_sample(g_Alpha_reader, &alpha_sub_p, NULL);

        usleep(250);
        sdds_History_print (DataReader_history (g_Alpha_reader));
        getchar();

        printf ("=> DEQUEUE\n");
        ret = DDS_AlphaDataReader_take_next_sample(g_Alpha_reader, &alpha_sub_p, NULL);

        usleep(250);
        sdds_History_print (DataReader_history (g_Alpha_reader));
        getchar();
    }

    return 0;
}
