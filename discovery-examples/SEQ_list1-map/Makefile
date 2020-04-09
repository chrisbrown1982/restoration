# make test creates a "results" file
#  dataSize   = atoi(argv[1]);
#  numWorkers = atoi(argv[2]);


LDFLAGS=-pthread

build : original final final-extend

test : original
	./original > ori_res
	./final    > fin_res
	diff  ori_res  fin_res

clean :
	rm original final final-extend   ori_res fin_res  -f
