
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "linux_sdds_impl.h"

#include <sdds/DataSink.h>
#include <sdds/DataSource.h>
#include <dds/DDS_DCPS.h>

#define COM_READ 0
#define COM_SW_ON 1
#define COM_SW_OFF 2
#define COM_SW_TG 3
#define COM_SW_ST 4
#define COM_QUIT 5
#define COM_HELP 6
#define COM_LAMP_READ 7


char* commands[] = {"read watt\n", "switch on\n", "switch off\n",
		"switch toggle\n", "switch state\n", "quit\n", "help\n", "read lamp\n", NULL};

int main(void)
{
	sDDS_init();
	Log_setLvl(0);

	for (;;)
	{
		printf("Please enter a command (or help) \n");


		size_t siz = 100;
		char* s = malloc(siz +1);
		ssize_t byte_read = getline(&s, &siz, stdin);

		if (byte_read < 0) {
			printf("Error: cant read from stdin\n");
			return -1;
		}

		if (strcmp(s, commands[COM_READ]) == 0)
		{
			Wattage t;
			Wattage* t_ptr = &t;

			if (DDS_WattageDataReader_take_next_sample(g_Wattage_reader, &t_ptr, NULL) != DDS_RETCODE_NO_DATA)
			{
				printf("wattage of node 0x%x: %d watt \n", (t.id), t.watt );
			}
		}
		else if (strcmp(s, commands[COM_SW_ON]) == 0)
		{
		    Switch_control switch_state;
		    switch_state.command = 1;
		    switch_state.id = 0x81a5;

		    if (DDS_Switch_controlDataWriter_write(g_Switch_control_writer, &switch_state, NULL) == DDS_RETCODE_OK) {
			printf("Command on send to switch 0x%4x \n", switch_state.id);

		    } else {
			printf("Error sending command on to switch 0x%4x \n", switch_state.id);

		    }
		}
		else if (strcmp(s, commands[COM_SW_OFF]) == 0)
		{
		    Switch_control switch_state;
		    switch_state.command = 0;
		    switch_state.id = 0x81a5;

		    if (DDS_Switch_controlDataWriter_write(g_Switch_control_writer, &switch_state, NULL) == DDS_RETCODE_OK) {
			printf("Command off send to switch 0x%4x \n", switch_state.id);

		    } else {
			printf("Error sending command off to switch 0x%4x \n", switch_state.id);

		    }
		}
		else if (strcmp(s, commands[COM_SW_TG]) == 0)
				{
				    Switch_control switch_state;
				    switch_state.command = 2;
				    switch_state.id = 0x81a5;

				    if (DDS_Switch_controlDataWriter_write(g_Switch_control_writer, &switch_state, NULL) == DDS_RETCODE_OK) {
					printf("Command toggle send to switch 0x%4x \n", switch_state.id);

				    } else {
					printf("Error sending command toggle to switch 0x%4x \n", switch_state.id);

				    }
				}
		else if (strcmp(s, commands[COM_SW_ST]) == 0)
						{
						    Switch_control switch_state;
						    switch_state.command = 3;
						    switch_state.id = 0x81a5;

						    if (DDS_Switch_controlDataWriter_write(g_Switch_control_writer, &switch_state, NULL) == DDS_RETCODE_OK) {
							printf("Command get state send to switch 0x%4x \n", switch_state.id);

						    } else {
							printf("Error sending command get state to switch 0x%4x \n", switch_state.id);

						    }
						}
		else if (strcmp(s, commands[COM_LAMP_READ]) == 0)
		{
			Lamp l;

			Lamp* l_ptr = &l;

			DDS_ReturnCode_t ret = DDS_LampDataReader_take_next_sample(g_Lamp_reader, &l_ptr, NULL);

			if (ret != DDS_RETCODE_NO_DATA) {
				printf("Lamp state of node 0x%x: %d\n", l.id, l.state);
			} else {
				printf("Not data aviable\n");
			}


		}
		else if (strcmp(s, commands[COM_QUIT]) == 0)
		{
			break;
		}
		else if (strcmp(s, commands[COM_HELP]) == 0)
		{
			int i = 0;
			char* out = commands[i];
			printf("Possible Commands: \n");
			while (out != NULL) {
				printf("\t %s", out);
				i++;
				out = commands[i];
			}

			printf("\n");
		}
		else
		{
			printf("unknown command: %s \n", s);
		}

		free(s);

	}

	return 0;
}
