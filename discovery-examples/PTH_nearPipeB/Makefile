# make test creates a "results" file

LDFLAGS=-pthread

build : original

test : original
	./original 2 && cat results

original : original.c


clean :
	rm original result -f
