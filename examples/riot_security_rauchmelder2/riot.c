
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

int main(void)
{

	DDS_ReturnCode_t ret;
  Alarm alarm_pub;
  TemperatureSensor temp_pub;
  int i;
  char *date = "19.04.2016.12:33";

	if (sDDS_init() == SDDS_RT_FAIL) {
		return 1;
	}

	Log_setLvl(0);

  xtimer_usleep(5 *SEC_IN_USEC);
  Security_receive_key();
  
  alarm_pub.id = 2;
  strncpy(alarm_pub.date, date, strlen(date) + 1);

  temp_pub.id = 123;

  for (i = 0;; i++) {  
    alarm_pub.state = 0;  
    temp_pub.temperature = 23;
    DDS_AlarmDataWriter_write(g_Alarm_writer, &alarm_pub, NULL);
    DDS_TemperatureSensorDataWriter_write(g_TemperatureSensor_writer, &temp_pub, NULL);
    xtimer_usleep(2 *SEC_IN_USEC);
  }

  return 0;

}
