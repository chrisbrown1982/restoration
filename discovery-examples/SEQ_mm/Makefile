TRACER_DIR= ../trace-pattern-finder/PatternPlayGround/Tracer/DataFlowSanitizer/Automated
MINIZINC_FINDER_DIR= ../trace-pattern-finder/PatternPlayGround/PatternFinder/MiniZinc

include $(TRACER_DIR)/Makefile

PROCESS_TRACE= $(TRACER_DIR)/process-trace.py
MINIZINC_FLAGS = -a --solver chuffed

LOOP1_FLAGS= --filter-mark 1
LOOP2_FLAGS= --filter-mark 2
LOOP3_FLAGS= --filter-mark 3

all-pdf: mm.loop1.simple.pdf mm.loop2.simple.pdf mm.loop3.simple.pdf

all-dzn: mm.loop1.dzn mm.loop2.dzn mm.loop3.dzn

all-patterns: mm.loop1.maps.pdf mm.loop2.maps.pdf mm.loop3.reductions.pdf

%.trace: %.bin
	./$<
	mv trace $@

mm.bin: original-instrumented.cc
	$(CXX) $^ -o $@ -g $(CBASEFLAGS) $(CDFSANFLAGS)

mm.loop1.simple.trace: mm.simple.trace
	$(PROCESS_TRACE) transform $(LOOP1_FLAGS) $< > $@

mm.loop2.simple.trace: mm.simple.trace
	$(PROCESS_TRACE) transform $(LOOP2_FLAGS) $< > $@

mm.loop3.simple.trace: mm.simple.trace
	$(PROCESS_TRACE) transform $(LOOP3_FLAGS) $< > $@
