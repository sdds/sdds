/*
 * =====================================================================================
 *
 *       Filename:  App.c
 *
 *    Description:  Sample dev application for the sDDS middleware
 *
 *        Version:  1.0
 *        Created:  22.02.2010 15:28:54
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */



#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "Foo.h"
#include "sdds/Log.h"
#include "temper.h"

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
int
main ( int argc, char *argv[] )
{

    DDS_init();
    Log_debug("DDS initiliesed\n");

    Foo data;
    Foo* data_ptr = &data;

    /*  x
    int fd;
    fd = temper_open("/dev/ttyUSB0");
    if (fd < 0) {
	Log_error("Can't open /dev/ttyUSB0\n");
	exit(1);
    }
     if (temper_init(fd) < 0) {
	 Log_error("Can't init the usb temp sensor\n");
	exit(1);
    }
    */



    for (int i = 0; i < 100; i++){
	static double t = 1;
	/*  
	if (temper_read(fd, &t) < 0) {
	    Log_error("Error reading temp sensor \n");
	    exit(1);
    	}
	*/
	t += 0.5;
	data.temperatur = (int) (t*10.0);
	printf("read %g %i C \n", t, data.temperatur);

	DDS_FooDataWriter_write( fooDataWriter, &data, NULL);
	sleep(5);
	Foo dataIn;
	Foo* dataIn_ptr = &dataIn;
	DDS_ReturnCode_t ret = DDS_FooDataReader_take_next_sample( 
		fooDataReader, &dataIn_ptr, NULL);
	if (ret == DDS_RETCODE_OK){
	    printf("Received Data: \n \t Temperatur: %i,%i Grad \n", 
		    (data.temperatur/10), (dataIn.temperatur%10));
	}
	sleep(10);
//	BuiltinTopic_publishAll();
    }

    return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */
