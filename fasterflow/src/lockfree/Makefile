all : lffarm lffarm2 lfpipe lfpipe2

lffarm : testlffarm.o lffarm.o  payload.o queue.h
	gcc -o lffarm testlffarm.o lffarm.o payload.o -lpthread

lffarm2 : testlffarm2.o lffarm2.o  payload.o queue.h
	gcc -o lffarm2 testlffarm2.o lffarm2.o payload.o -lpthread

lfpipe : testpipe.o lffarm.o payload.o queue.h
	gcc -o lfpipe testpipe.o lffarm.o payload.o  -lpthread

lfpipe2 : testpipe2.o lffarm.o payload.o queue.h
	gcc -o lfpipe2 testpipe2.o lffarm2.o payload.o  -lpthread

clean :
	-rm lffarm lffarm2 lfpipe lfpipe2
	-rm  testlffarm.o testlffarm2.o 
	-rm  testpipe.o testpipe2.o
	-rm lffarm.o lffarm2.o
	-rm payload.o

%.o : %.c
	$(CC) -c -w -O2 $<

