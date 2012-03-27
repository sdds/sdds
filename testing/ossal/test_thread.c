/*
 * thread.c
 *
 *  Created on: Jul 19, 2010
 *      Author: kai
 */

#include "os-ssal/Thread.h"
#include "stdio.h"

void* foo(void* d) {

	printf("Hello world : %s\n", (char*)d);
	return NULL;
}

int main (int argv, char** argc) {

	Thread t = Thread_create();
	char* d = "moeoep";

	Thread_init(t, foo, d);

	return 0;
}

