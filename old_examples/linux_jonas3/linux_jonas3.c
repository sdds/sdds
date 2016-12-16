#include <unistd.h>
#include "linux_jonas3_sdds_impl.h"

int main() {
	DDS_ReturnCode_t ret;
  Alarm alarm_pub;
  int i;
  char *date = "19.04.2016.12:33";

	if (sDDS_init() == SDDS_RT_FAIL) {
		return 1;
	}

	Log_setLvl(0);

  sleep(1);
  Security_receive_key();

  strncpy(alarm_pub.date, date, strlen(date) + 1);

  for (i = 0; i < 10; i++) {
    if(i % 2) {
      alarm_pub.state = 0;  
    } else {
      alarm_pub.state = 1;  
    }
    ret = DDS_AlarmDataWriter_write(g_Alarm_writer, &alarm_pub, NULL);
    sleep (5);
  }

  return 0;
}
