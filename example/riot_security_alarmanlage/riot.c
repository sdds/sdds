
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include "riot_sdds_impl.h"
#include "alarm-ds.h"
#include "os-ssal/Thread.h"
#include "xtimer.h"
#include "thread.h"

int main(void);

int main(void) {

	DDS_ReturnCode_t ret;
  Alarm alarm_sub;
  Alarm *alarm_sub_p = &alarm_sub;
  TemperatureSensor temp_sub;
  TemperatureSensor *temp_sub_p = &temp_sub;
  int i;

	if (sDDS_init() == SDDS_RT_FAIL) {
		return 1;
	}

	Log_setLvl(0);

  xtimer_usleep(5 *SEC_IN_USEC);
  Security_receive_key();

  for (i = 0;;i++) {

      ret = DDS_AlarmDataReader_take_next_sample(g_Alarm_reader, &alarm_sub_p, NULL);

      if (ret == DDS_RETCODE_NO_DATA) {
          if(!(i%5)) {printf("no data for alarm\n");}
      } else {
          printf("Received a sample from topic 'Alarm': {\n"
                 "   id => %d\n"
                 "   state => %d\n"
                 "   date => %s\n"
                 "}\n"
                 , alarm_sub_p->id
                 , alarm_sub_p->state
                 , alarm_sub_p->date
          );
      }

      ret = DDS_TemperatureSensorDataReader_take_next_sample(g_TemperatureSensor_reader, &temp_sub_p, NULL);

      if (ret == DDS_RETCODE_NO_DATA) {
          if(!(i%5)) {printf("no data for temperature\n");}
      } else {
          printf("Received a sample from topic 'TemperatureSensor': {\n"
                 "   id => %d\n"
                 "   temperature => %d\n"
                 "}\n"
                 , temp_sub_p->id
                 , temp_sub_p->temperature
          );
      }

      xtimer_usleep(1 *SEC_IN_USEC);

  }
  

  return 0;
}
