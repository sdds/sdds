
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
  int i;
  char *date = "19.04.2016.12:33";
  uint32_t t;

  xtimer_init();	

  t = xtimer_now();
	if (sDDS_init() == SDDS_RT_FAIL) {
		return 1;
	}
  printf("init time elapsed us: %ld\n", xtimer_now() - t);  

	Log_setLvl(0);

  xtimer_usleep(5 *SEC_IN_USEC);

#ifdef FEATURE_SDDS_SECURITY_ENABLED
  Security_receive_key();
#endif

  alarm_pub.id = 1;
  strncpy(alarm_pub.date, date, strlen(date) + 1);

  //t = xtimer_now();
  for (i = 0;; i++) {
    alarm_pub.state = 1;  
    DDS_AlarmDataWriter_write(g_Alarm_writer, &alarm_pub, NULL);
    xtimer_usleep(2 *SEC_IN_USEC);
  }
  //printf("time elapsed us: %ld\n", xtimer_now() - t);  

  return 0;

}
