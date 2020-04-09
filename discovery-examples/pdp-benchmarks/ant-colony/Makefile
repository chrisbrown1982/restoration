PATTERN_PLAYGROUND_DIR=../../trace-pattern-finder/PatternPlayGround
TRACER_DIR= $(PATTERN_PLAYGROUND_DIR)/Tracer/DataFlowSanitizer/Automated
MINIZINC_FINDER_DIR= $(PATTERN_PLAYGROUND_DIR)/PatternFinder/MiniZinc

# Placed here to override the recipe in $(TRACER_DIR)/Makefile.
%.dzn: %.input
	$(PROCESS_TRACE) --output-format=minizinc --match-regions-only print $< > $@

include $(TRACER_DIR)/Makefile
include ../common.mk

CXXFLAGS += -std=c++0x

# Internal loop IDs for trace filtering, see 'mark' enum in source code.
LOOP258= 0
LOOP259= 1
LOOP120= 2
LOOP123= 3
LOOP211= 4
LOOP90=  5
LOOP212= 6
LOOP134= 7

BENCHMARK_ARGS= 4 4 ../input/wt5.txt

BENCHMARK_LOOPS= 258 259 120 123 211 90 212 134
ALL_PREFIX= $(addprefix ant-colony.loop, $(BENCHMARK_LOOPS))

all-maps-szn: $(ALL_MAPS_SZN)
all-reductions-szn: $(ALL_REDUCTIONS_SZN)
all-trace-pdf: $(ALL_TRACE_PDF)
all-maps-pdf: $(ALL_MAPS_PDF)
all-reductions-pdf: $(ALL_REDUCTIONS_PDF)

ant-colony.loop%.bin: src/ant_seq-instrumented.cpp tracing.loop%.o
	$(CXX) $(CXXFLAGS) $^ -o $@
