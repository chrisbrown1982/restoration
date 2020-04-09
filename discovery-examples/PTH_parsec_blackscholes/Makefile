LDFLAGS := -pthread -lm

build: original

test : original
	./original 2 input16.txt test


original:  original.cpp

inputgen:  inputgen.cpp


clean :
	rm -f test original
