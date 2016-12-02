#include "contiki.h"
#include "os-ssal/Thread.h"

ssw_rc_t Thread_usleep(Thread _this, uint16_t usecSleep) {
  clock_delay_usec(usecSleep);
  return SDDS_SSW_RT_OK;
}

ssw_rc_t
Thread_sleep(Thread _this, uint16_t secSleep) {
  clock_wait(secSleep * CLOCK_SECOND);
}

