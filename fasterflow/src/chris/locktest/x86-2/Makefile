CC=gcc

all : farm farm2 farm3 farm4 farm5 pipe pipe2

farm : testfarmmain.o farm.o xchg-spinlock.o payload.o
	gcc -o farm testfarmmain.o farm.o xchg-spinlock.o payload.o -lpthread

farm2 : testfarmmain2.o farm.o xchg-spinlock.o payload.o
	gcc -o farm2 testfarmmain2.o farm.o xchg-spinlock.o payload.o -lpthread

farm3 : testfarmmain3.o farm2.o  payload.o xchg-spinlock2.o
	gcc -o farm3 testfarmmain3.o farm2.o payload.o xchg-spinlock2.o -lpthread

farm4 : testfarmmain3.o farm3.o  payload.o xchg-spinlock2.o
	gcc -o farm4 testfarmmain3.o farm3.o payload.o xchg-spinlock2.o -lpthread

farm5 : testfarmmain3.o farm4.o  payload.o
	gcc -o farm5 testfarmmain3.o farm4.o payload.o -lpthread

#convolution:	convolution.o
#	gcc -o -I/usr/include/libpng12 -I/home/chris/convolution -L/usr/lib64 convolution convolution.cpp payload.o -lpthread -lpng12 -lOpenCL

pipe : testpipe.o farm2.o payload.o xchg-spinlock2.o
	gcc -o pipe testpipe.o farm2.o payload.o xchg-spinlock2.o  -lpthread

pipe2 : testpipe.o farm3.o payload.o xchg-spinlock2.o
	gcc -o pipe2 testpipe.o farm3.o payload.o xchg-spinlock2.o  -lpthread

clean :
	rm farm testfarmmain.o farm.o xchg-spinlock.o payload.o
	rm farm2 testfarmmain2.o
	rm farm3 testfarmmain3.o farm2.o xchg-spinlock2.o payload.o
	rm pipe testpipe.o farm2.o xchg-spinlock2.o payload.o -lpthread
	rm farm4 farm3.o
	rm farm5 farm4.o

%.o : %.c
	$(CC) $< -c -O2

