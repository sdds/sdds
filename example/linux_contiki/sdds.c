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

    DDS_ReturnCode_t ret;

    Beta beta_data_used;
    Beta* beta_data_used_ptr = &beta_data_used;

    DDS_DCPSParticipant p_data_used;
    DDS_DCPSParticipant* p_data_used_ptr = &p_data_used;

    DDS_DCPSTopic t_data_used;
    DDS_DCPSTopic* t_data_used_ptr = &t_data_used;

	for (;;)
	{

		ret = DDS_DCPSParticipantDataReader_take_next_sample(g_DCPSParticipant_reader, &p_data_used_ptr, NULL);
    if (ret == DDS_RETCODE_NO_DATA)
    {
        printf("no data participant\n");
    }
    else
    { 
        printf("Received (participant):[%c]\n", p_data_used.key);
    }


    ret = DDS_DCPSTopicDataReader_take_next_sample(g_DCPSTopic_reader, &t_data_used_ptr, NULL);
    if (ret == DDS_RETCODE_NO_DATA)  
    {
        printf("no data topic\n");
    }
    else
    { 
        printf("Received (topic):[%c] topic:%s; type:%s\n", t_data_used.key, t_data_used.name, t_data_used.type_name);
    }

        ret = DDS_BetaDataReader_take_next_sample(g_Beta_reader, &beta_data_used_ptr, NULL);
      	if (ret == DDS_RETCODE_NO_DATA)
		{
            printf("no data beta\n");
		}
		else
		{ 
			printf("Received (beta):[%c] %s %s\n", beta_data_used.value, beta_data_used.value2, beta_data_used.value3);
	    }
    
        sleep(10);

    }
}
