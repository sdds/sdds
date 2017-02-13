
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "mywrite_sdds_impl.h"

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

		if (strcmp(s, "switch_off\n") == 0)
		{
			OnOffFunctionality t;
			OnOffFunctionality* t_ptr = &t;

			t.command = 0;
			t.id = 0x18aa;

			if (DDS_OnOffFunctionalityDataWriter_write(g_OnOffFunctionality_writer, t_ptr, NULL) != DDS_RETCODE_NO_DATA)
			{
				printf("test value: %d device 0x%x \n", (t.command), t.id );
			}
		} else if (strcmp(s, "switch_on\n") == 0)
		{
			OnOffFunctionality t;
			OnOffFunctionality* t_ptr = &t;

			t.command = 1;
			t.id = 0x18aa;

			if (DDS_OnOffFunctionalityDataWriter_write(g_OnOffFunctionality_writer, t_ptr, NULL) != DDS_RETCODE_NO_DATA)
			{
				printf("test value: %d device 0x%x \n", (t.command), t.id );
			}
		}
		else if (strcmp(s, "dim100\n") == 0)
				{
					LightRegulationFunctionality t;
					LightRegulationFunctionality* t_ptr = &t;

					t.setValue = 100;
					t.id = 0x18aa;

					if (DDS_LightRegulationFunctionalityDataWriter_write(g_LightRegulationFunctionality_writer, t_ptr, NULL) != DDS_RETCODE_NO_DATA)
					{
						printf("test value: %d device 0x%x \n", (t.setValue), t.id );
					}
				}
		else if (strcmp(s, "dim50\n") == 0)
						{
							LightRegulationFunctionality t;
							LightRegulationFunctionality* t_ptr = &t;

							t.setValue = 50;
							t.id = 0x18aa;

							if (DDS_LightRegulationFunctionalityDataWriter_write(g_LightRegulationFunctionality_writer, t_ptr, NULL) != DDS_RETCODE_NO_DATA)
							{
								printf("test value: %d device 0x%x \n", (t.setValue), t.id );
							}
						}
		else if (strcmp(s, "dim0\n") == 0)
								{
									LightRegulationFunctionality t;
									LightRegulationFunctionality* t_ptr = &t;

									t.setValue = 0;
									t.id = 0x18aa;

									if (DDS_LightRegulationFunctionalityDataWriter_write(g_LightRegulationFunctionality_writer, t_ptr, NULL) != DDS_RETCODE_NO_DATA)
									{
										printf("test value: %d device 0x%x \n", (t.setValue), t.id );
									}
								}
		else if (strcmp(s, "dim\n") == 0)
										{
											LightRegulationFunctionality t;
											LightRegulationFunctionality* t_ptr = &t;

											printf("Value: ");
											scanf("%u", &(t.setValue));
											t.id = 0x18aa;

											if (DDS_LightRegulationFunctionalityDataWriter_write(g_LightRegulationFunctionality_writer, t_ptr, NULL) != DDS_RETCODE_NO_DATA)
											{
												printf("test value: %d device 0x%x \n", (t.setValue), t.id );
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
