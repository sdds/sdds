include ../config.mk

OBJS:
	$(MAKE) -C $(ROOT)/src/sDDS/
	$(MAKE) -C $(PLATFORMPATH)/

clean:	
	$(MAKE) -C $(ROOT)/src/sDDS/ clean
	$(MAKE) -C $(PLATFORMPATH)/ clean
	rm -rf test_task.o testTask

test_task.o:
	$(CC) $(CFLAGS) $(HEADER)  $(DDSINCLUDE) -c ossal/test_task.c -o test_task.o

testTask: OBJS test_task.o
	$(CC) $(CFLAGS) $(HEADER) $(DDSINCLUDE) -o testTask test_task.o $(sDDSOBJ) $(PLATFORMOBJS)  -pthread -lrt
