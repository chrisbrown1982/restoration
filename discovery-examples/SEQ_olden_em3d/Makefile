
CC = g++
CFLAGS = -DTORONTO   -std=gnu++17  -Wall -O3 -fopenmp -g  -Wextra  -pedantic
TEST_PARAMETERS:= 1024 1024 128



build : build/use_stl_find_em3d build/original_em3d


test :  
	@make clean 
	@make run_original_em3d.txt 
	@make run_use_stl_find_em3d.txt 
	@diff run_use_stl_find_em3d.txt  run_original_em3d.txt


run_%.txt time_%.txt : build/%
	@echo  ./build/$* ${TEST_PARAMETERS} 
	@/bin/time --output=time_$*.txt -f "%P\t CPU Percentage\n%M\t Max Resident Set Size(Kb)\n%E\t Elapsed Real Time" ./build/$* ${TEST_PARAMETERS} > run_$*.txt
	@cat time_$*.txt


build/%_em3d : build/%/args.o build/%/make_graph.o build/%/util.o build/%/main.o build/%/em3d.o
	mkdir -p build
	${CC} ${CFLAGS} -o $@  $^	

build/%.o : %.c 
	mkdir -p `dirname $@`
	${CC} ${CFLAGS} -o $@ -c $<

build/%.o : %.cpp
	mkdir -p `dirname $@`
	${CC} ${CFLAGS} -o $@ -c $<




clean : 
	rm build/ *.txt  -rf

