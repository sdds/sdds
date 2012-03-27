/*
 * test_task.c
 *
 *  Created on: Jul 22, 2010
 *      Author: kai
 */
#include <stdio.h>
#include "os-ssal/Task.h"
#include "os-ssal/SSW.h"

#include <time.h>

void foo(void* obj){
    time_t curT = time(NULL);
    printf("hello world: %s\n", ctime(&curT));
}

int main (int argv, char** argc) {

    Task t = Task_create();

    ssw_rc_t rc;

    rc = Task_init(t, foo, NULL);
    time_t curT = time(NULL);
    printf("init: %s\n", ctime(&curT));

    rc = Task_start(t, 5, 0, SSW_TaskMode_single);
    curT = time(NULL);
    printf("started: %s\n", ctime(&curT));

    sleep(10);
    curT = time(NULL);
    printf("close: %s\n", ctime(&curT));

    return 0;
}

