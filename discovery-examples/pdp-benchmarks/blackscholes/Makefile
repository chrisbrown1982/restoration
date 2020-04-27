# PATTERN_PLAYGROUND_DIR=../../trace-pattern-finder/PatternPlayGround
# TRACER_DIR= $(PATTERN_PLAYGROUND_DIR)/Tracer/DataFlowSanitizer/Automated
# MINIZINC_FINDER_DIR= $(PATTERN_PLAYGROUND_DIR)/PatternFinder/MiniZinc

# # Placed here to override the recipe in $(TRACER_DIR)/Makefile.
# %.dzn: %.input
# 	$(PROCESS_TRACE) --output-format=minizinc --match-regions-only print $< > $@

# include $(TRACER_DIR)/Makefile
# include ../common.mk

# CXXFLAGS +=

# CXXLINKFLAGS = -lm

# # Internal loop IDs for trace filtering, see 'mark' enum in source code.
# LOOP285= 0
# LOOP290= 1
# LOOP373= 2

# BENCHMARK_ARGS= 1 ../input/in_4.txt prices.txt

# BENCHMARK_LOOPS= 285 290 373
# ALL_PREFIX= $(addprefix blackscholes.loop, $(BENCHMARK_LOOPS))

# all-maps-szn: $(ALL_MAPS_SZN)
# all-reductions-szn: $(ALL_REDUCTIONS_SZN)
# all-trace-pdf: $(ALL_TRACE_PDF)
# all-maps-pdf: $(ALL_MAPS_PDF)
# all-reductions-pdf: $(ALL_REDUCTIONS_PDF)

# blackscholes.loop%.bin: src/blackscholes-instrumented.c tracing.loop%.o
# 	$(CXX) $(CXXFLAGS) $(CXXLINKFLAGS) $^ -o $@

# foo.bin: src/blackscholes-instrumented.c
# 	$(CXX) $(CXXFLAGS) $(CXXLINKFLAGS) $^ -o $@

# CC=g++
CC=clang++

# INPUTFILE=inputs/in_4.txt
# INPUTFILE=inputs/in_16.txt
# INPUTFILE=inputs/in_4K.txt
INPUTFILE=inputs/in_64K.txt

all : seq pth

seq : ./src/blackscholes_seq.c
	$(CC) ./src/blackscholes_seq.c -o seq

pth : ./src/blackscholes_pth.c
	$(CC) ./src/blackscholes_pth.c -lpthread -o pth

inputs_test : ./inputs/input_test.tar
	tar -C ./inputs -xvf ./inputs/input_test.tar

runseq : $(INPUTFILE) seq
	./seq 1 $(INPUTFILE) out_seq.txt

runpth : $(INPUTFILE) pth
	./pth 2 $(INPUTFILE) out_pth.txt

pth.1 : ./src/blackscholes_pth.1.c
	$(CC) ./src/blackscholes_pth.1.c -lpthread -o pth.1
	
runpth.1 : $(INPUTFILE) pth.1
	./pth.1 2 $(INPUTFILE) out_pth.txt

pth.alt.1 : ./src/blackscholes_pth.alt.1.c
	$(CC) ./src/blackscholes_pth.alt.1.c -lpthread -o pth.alt.1
	
runpth.alt.1 : $(INPUTFILE) pth.alt.1
	./pth.alt.1 2 $(INPUTFILE) out_pth.alt.1.txt

pth.alt.2 : ./src/blackscholes_pth.alt.2.c
	$(CC) ./src/blackscholes_pth.alt.2.c -lpthread -ltbb -o pth.alt.2
	
runpth.alt.2 : $(INPUTFILE) pth.alt.2
	./pth.alt.2 2 $(INPUTFILE) out_pth.alt.2.txt

pth.2 : ./src/blackscholes_pth.2.c
	$(CC) ./src/blackscholes_pth.2.c -lpthread -o pth.2

runpth.2 : $(INPUTFILE) pth.2
	./pth.2 2 $(INPUTFILE) out_pth.txt

pth.3 : ./src/blackscholes_pth.3.c
	$(CC) ./src/blackscholes_pth.3.c -lpthread -o pth.3

runpth.3 : $(INPUTFILE) pth.3
	./pth.3 2 $(INPUTFILE) out_pth.txt

pth.4 : ./src/blackscholes_pth.4.c
	$(CC) ./src/blackscholes_pth.4.c -lpthread -o pth.4

runpth.4 : $(INPUTFILE) pth.4
	./pth.4 2 $(INPUTFILE) out_pth.txt

pth.5 : ./src/blackscholes_pth.5.c
	$(CC) ./src/blackscholes_pth.5.c -lpthread -o pth.5

runpth.5 : $(INPUTFILE) pth.5
	./pth.5 2 $(INPUTFILE) out_pth.txt

pth.6 : ./src/blackscholes_pth.6.cpp
	$(CC) ./src/blackscholes_pth.6.cpp -lpthread -ltbb -o pth.6

runpth.6 : $(INPUTFILE) pth.6
	./pth.6 2 $(INPUTFILE) out_pth.txt