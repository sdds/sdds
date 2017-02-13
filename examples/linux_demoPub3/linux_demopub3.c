#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "linux_demopub3_sdds_impl.h"
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

    for (;;) {
	printf("Eingabe (0 - 255): ");
	scanf("%"SCNd8, &red_pub.value);
	while (getchar()!='\n');
        ret = DDS_RedDataWriter_write (g_Red_writer, &red_pub, NULL);

        usleep (100000);
    }

    return 0;
}
