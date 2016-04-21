#include <unistd.h>
#include "linux_jonas2_sdds_impl.h"

int main() {
	DDS_ReturnCode_t ret;
  Alarm alarm_sub;
  Alarm *alarm_sub_p = &alarm_sub;
  int i;

	if (sDDS_init() == SDDS_RT_FAIL) {
		return 1;
	}

	Log_setLvl(0);

  sleep(1);
  //Security_receive_key();

  for (i = 0; i < 10; i++) {

      ret = DDS_AlarmDataReader_take_next_sample(g_Alarm_reader, &alarm_sub_p, NULL);

      if (ret == DDS_RETCODE_NO_DATA) {
          printf("no data for alarm\n");
      } else {
          printf("Received a sample from topic 'Alarm': {\n"
                 "   state => %d\n"
                 "   date => %s\n"
                 "}\n"
                 , alarm_sub_p->state
                 , alarm_sub_p->date
          );
      }

      sleep (5);

  }
  

  return 0;
}
