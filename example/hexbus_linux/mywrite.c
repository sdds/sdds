
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
#include <sdds/Log.h>

#define COM_READ 0
#define COM_SW_ON 1
#define COM_SW_OFF 2
#define COM_SW_TG 3
#define COM_SW_ST 4
#define COM_QUIT 5
#define COM_HELP 6
#define COM_LAMP_ST 7

#define RECEIVER_ID 0x81a5

char* commands[] = {"read watt\n", "switch on\n", "switch off\n",
		"switch toggle\n", "switch state\n", "quit\n", "help\n", "lamp state\n", NULL};

void _readPowerMeter();
void _setOnOffSwitch(bool state);
void _toggleOnOffSwitch();
void _readOnOffSwitch();
void _readSimpleLamp();

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
			_readPowerMeter();
		}
		else if (strcmp(s, commands[COM_SW_ON]) == 0)
		{
			_setOnOffSwitch(true);

		}
		else if (strcmp(s, commands[COM_SW_OFF]) == 0)
		{
			_setOnOffSwitch(false);
		}
		else if (strcmp(s, commands[COM_SW_TG]) == 0)
		{
			_toggleOnOffSwitch();
		}
		else if (strcmp(s, commands[COM_SW_ST]) == 0)
		{

		}
		else if (strcmp(s, commands[COM_LAMP_ST]) == 0)
		{
			_readSimpleLamp();
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

void _readSimpleLamp()
{
	SimpleLamp st;
	SimpleLamp* st_ptr = &st;

	DDS_ReturnCode_t ret;

	ret = DDS_SimpleLampDataReader_take_next_sample(g_SimpleLamp_reader, &st_ptr, NULL);

	if (ret == DDS_RETCODE_NO_DATA)
		return;

	if (ret != DDS_RETCODE_OK) {
		printf("Error getting next sample for topic SimpleLamp, retcode: %d\n", ret);
		return;
	}

	printf("Status of SimpleLamp is %s for node 0x%4x\n", (st.state ? "on" : "off"), st.id);
}


void _setOnOffSwitch(bool state)
{
	OnOffFunctionality func;

	if (state == true) {
		func.command = 1;
	} else {
		func.command = 0;
	}
	func.dummy = 0;
	func.id = RECEIVER_ID;

	DDS_ReturnCode_t ret;

	ret = DDS_OnOffFunctionalityDataWriter_write(g_OnOffFunctionality_writer, &func, NULL);

	if (ret != DDS_RETCODE_OK) {
		printf("Error publishing OnOffFunctionality topic to node 0x%4x \n", func.id);
	}
}

void _toggleOnOffSwitch()
{
	ToggleFunctionality tog;

	tog.command = 0;
	tog.dummy = 0;
	tog.id = RECEIVER_ID;

	DDS_ReturnCode_t ret;

	ret = DDS_ToggleFunctionalityDataWriter_write(g_ToggleFunctionality_writer,&tog, NULL);

	if (ret != DDS_RETCODE_OK) {
		printf("Error publishing ToggleFunctionality topic to node 0x%4x  \n", tog.id);
	}
}

void _readOnOffSwitch()
{
	OnOffSwitch st;
	OnOffSwitch* st_ptr = &st;

	DDS_ReturnCode_t ret;

	ret = DDS_OnOffSwitchDataReader_take_next_sample(g_OnOffSwitch_reader, &st_ptr, NULL);

	if (ret == DDS_RETCODE_NO_DATA)
		return;

	if (ret != DDS_RETCODE_OK) {
		printf("Error getting next sample for topic OnOffSwitch, retcode: %d\n", ret);
		return;
	}

	printf("Status of OnOffSwitch is %s for node 0x%4x\n", (st.state ? "on" : "off"), st.id);
}

void _readPowerMeter()
{
	PowerMeter power;
	PowerMeter* power_ptr = &power;

	DDS_ReturnCode_t ret;
	ret = DDS_PowerMeterDataReader_take_next_sample(g_PowerMeter_reader, &power_ptr, NULL);

	if (ret == DDS_RETCODE_NO_DATA)
		return;

	if (ret != DDS_RETCODE_OK) {
		printf("Error getting next sample for topic PowerMeter, retcode: %d\n", ret);
		return;
	}


	printf("Power consumption of node 0x%x: %d watt \n", (power.id), power.watt );

}
