#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include "linux_sdds_impl.h"
#include "beta-ds.h"
#include "BuiltinTopic.h"

int main()
{
    pthread_t t;

	sDDS_init();
    Log_setLvl(0);

	for (;;) {

        do
		{
			static uint8_t foo = 67;
            DDS_DCPSParticipant p;
            p.key = foo;
           
            DDS_DCPSTopic t;
            t.key = foo;
            t.name[0] = 'T';
            t.name[1] = 'e';
            t.name[2] = 'm';
            t.name[3] = 'p';
            t.name[4] = '\0';
            t.type_name[0] = 'T';
            t.type_name[1] = '1';
            t.type_name[2] = '\0';

            Beta b;
            b.value = foo++;
            b.value2[0] = 'D';
            b.value2[1] = 'e';
            b.value2[2] = 'r';
            b.value2[3] = '\0';
            b.value3[0] = 'B';
            b.value3[1] = 'a';
            b.value3[2] = 'u';
            b.value3[3] = 'm';
            b.value3[4] = '!';
            b.value3[5] = '\0';



			if (DDS_DCPSParticipantDataWriter_write(g_DCPSParticipant_writer, &p, NULL) != DDS_RETCODE_OK) {
                // handle error
            }
            if (DDS_DCPSTopicDataWriter_write(g_DCPSTopic_writer, &t, NULL) != DDS_RETCODE_OK) {
                // handle error
            }
            if (DDS_BetaDataWriter_write(g_Beta_writer, &b, NULL) != DDS_RETCODE_OK) {
                // handle error
            }
            sleep(10);

		} while(0);

    }

}
