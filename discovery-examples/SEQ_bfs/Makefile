# make test creates a "results" file
#  dataSize   = atoi(argv[1]);
#  numWorkers = atoi(argv[2]);


LDFLAGS=-pthread

build : original
original : original.cc

test : original
	./original 

clean :
	rm original  -f
