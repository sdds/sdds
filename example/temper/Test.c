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
#include "Foo.h"
#include "sDDS/Log.h"
#include "sDDS/Marshalling.h"

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
int
main ( int argc, char *argv[] )
{
//    DDS_ReturnCode_t rc;

    DDS_long l1 = 15;
    byte_t buff[4];

    byte_t* tmp = &l1;

    printf("Bytes \n0 \t 1 \t 2 \t 3 \t \n");
    for (int i = 0; i < sizeof(DDS_long); i++)
	printf("%02x \t ", tmp[i]);
    printf("\n");

    Marshalling_encodeLong(buff, &l1);
    printf("Bytes \n0 \t 1 \t 2 \t 3 \t \n");
    for (int i = 0; i < sizeof(DDS_long); i++)
	printf("%x \t ", buff[i]);
    printf("\n");

  //  DDS_init();
    Log_debug("DDS initiliesed\n");

//    Foo data;
 //   data.temperatur = 23;
  //  data.id = 1;

	
 //   DDS_FooDataWriter_write( fooDataWriter, &data, NULL);

    return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */
