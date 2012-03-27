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
    DDS_ReturnCode_t rc;

    DDS_init();
    Log_debug("DDS initiliesed\n");

    

    while (true) {
	
	
    }

    return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */
