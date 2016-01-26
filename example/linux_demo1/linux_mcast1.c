#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "linux_mcast1_sdds_impl.h"
#include "unistd.h"

int main()
{
	DDS_ReturnCode_t ret;
	Log_setLvl(4);

	if (sDDS_init() == SDDS_RT_FAIL) {
		return 1;
	}

    Red red_pub;
    red_pub.value = 0;

//    Green green_pub;
//    green_pub.value = 85;
//
//    Blue blue_pub;
//    blue_pub.value = 170;

    for (;;) {
	printf("Eingabe (0 - 255): ");
	scanf("%d", &red_pub.value);
	while (getchar()!='\n');
        ret = DDS_RedDataWriter_write (g_Red_writer, &red_pub, NULL);

//    	ret = DDS_GreenDataWriter_write (g_Green_writer, &green_pub, NULL);
//        green_pub.value++;
//
//        ret = DDS_BlueDataWriter_write (g_Blue_writer, &blue_pub, NULL);
//        blue_pub.value++;
        usleep (100000);
    }

    return 0;
}
