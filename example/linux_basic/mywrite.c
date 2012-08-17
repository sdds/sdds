
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "linux_sdds_impl.h"

#include <sdds/DataSink.h>
#include <sdds/DataSource.h>


int main(void)
{
	sDDS_init();

	for (;;)
	{
		printf("Please enter a command\n");


		size_t siz = 100;
		char* s = malloc(siz +1);
		ssize_t byte_read = getline(&s, &siz, stdin);

		if (byte_read < 0) {
			printf("Error: cant read from stdin\n");
			return -1;
		}

		if (strcmp(s, "read_test\n") == 0)
		{
			Test t;
			Test* t_ptr = &t;

			if (DDS_TestDataReader_take_next_sample(g_Test_reader, &t_ptr, NULL) != DDS_RETCODE_NO_DATA)
			{
				printf("test value: %d \n", (t.testvalue) );
			}
		}
		else if (strcmp(s, "quit\n") == 0)
		{
			break;
		}
		else
		{
			printf("unknown command: %s \n", s);
		}

		free(s);

	}

	return 0;
}
