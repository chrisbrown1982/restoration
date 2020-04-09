# make test creates a "results" file
#  dataSize   = atoi(argv[1]);
#  numWorkers = atoi(argv[2]);


LDFLAGS=-pthread

build : original final seq

test : original
	./original 1000 16 && mv results ori_res
	./final    1000 16 && mv results fin_res
	diff  ori_res  fin_res

clean :
	rm original final seq  results  ori_res fin_res  -f
