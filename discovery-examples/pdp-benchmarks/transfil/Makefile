PATTERN_PLAYGROUND_DIR=../../trace-pattern-finder/PatternPlayGround
TRACER_DIR= $(PATTERN_PLAYGROUND_DIR)/Tracer/DataFlowSanitizer/Automated
MINIZINC_FINDER_DIR= $(PATTERN_PLAYGROUND_DIR)/PatternFinder/MiniZinc

# Placed here to override the recipe in $(TRACER_DIR)/Makefile.
%.dzn: %.input
	$(PROCESS_TRACE) --output-format=minizinc --match-regions-only print $< > $@

include $(TRACER_DIR)/Makefile
include ../common.mk

CXXFLAGS += -std=c++11

CXXLINKFLAGS = -lgsl -lgslcblas

# Internal loop IDs for trace filtering, see 'mark' enum in source code.
LOOP54=  0
LOOP233= 1
LOOP94=  2
LOOP78=  3
LOOP157= 4
LOOP183= 5
LOOP90=  6
LOOP120= 7
LOOP217= 8
LOOP464= 9

BENCHMARK_ARGS= 1 -s ../input/NWscenarios-tracing.txt -n ../input/Pop_Distribution_NW-tracing.csv -r 4 -t 12

BENCHMARK_LOOPS= 54 233 94 78 157 183 90 120 217 464
ALL_PREFIX= $(addprefix transfil.loop, $(BENCHMARK_LOOPS))

all-maps-szn: $(ALL_MAPS_SZN)
all-reductions-szn: $(ALL_REDUCTIONS_SZN)
all-trace-pdf: $(ALL_TRACE_PDF)
all-maps-pdf: $(ALL_MAPS_PDF)
all-reductions-pdf: $(ALL_REDUCTIONS_PDF)

%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $^ -c -o $@

# FIXME: investigate final segmentation fault (occurs after main()'s execution
# so does not affect our analysis)
transfil.loop%.bin: main-instrumented.o ScenariosList.o Scenario.o Population-instrumented.o Host.o Model-instrumented.o Output.o Vector-instrumented.o Worm.o Statistics.o MDAEvent.o loops.o tracing.loop%.o
	$(CXX) $(CXXFLAGS) $(CXXLINKFLAGS) -DTRACE_REGION=$(LOOP$*) -lgsl -lgslcblas $^ -o $@
