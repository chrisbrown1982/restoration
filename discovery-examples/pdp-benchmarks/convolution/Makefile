# PATTERN_PLAYGROUND_DIR=../../trace-pattern-finder/PatternPlayGround
# TRACER_DIR= $(PATTERN_PLAYGROUND_DIR)/Tracer/DataFlowSanitizer/Automated
# MINIZINC_FINDER_DIR= $(PATTERN_PLAYGROUND_DIR)/PatternFinder/MiniZinc

# # Placed here to override the recipe in $(TRACER_DIR)/Makefile.
# %.dzn: %.input
# 	$(PROCESS_TRACE) --output-format=minizinc --match-regions-only print $< > $@

# include $(TRACER_DIR)/Makefile
# include ../common.mk

# CXXFLAGS += -std=c++1y -lpng

# CXXLINKFLAGS = -lpng

# # Internal loop IDs for trace filtering, see 'mark' enum in source code.
# LOOP158= 0
# LOOP111= 1
# LOOP112= 2
# LOOP119= 3
# LOOP120= 4

# BENCHMARK_ARGS=

# BENCHMARK_LOOPS= 158 111 112 119 120
# ALL_PREFIX= $(addprefix convolution.loop, $(BENCHMARK_LOOPS))

# all-maps-szn: $(ALL_MAPS_SZN)
# all-reductions-szn: $(ALL_REDUCTIONS_SZN)
# all-trace-pdf: $(ALL_TRACE_PDF)
# all-maps-pdf: $(ALL_MAPS_PDF)
# all-reductions-pdf: $(ALL_REDUCTIONS_PDF)

# convolution.loop%.bin: src/convolution_seq-instrumented.cpp tracing.loop%.o
# 	$(CXX) $(CXXFLAGS) $(CXXLINKFLAGS) $^ -o $@


all : seq

seq : ./src/convolution_seq.cpp
	g++ ./src/convolution_seq.cpp -lpng -o seq
