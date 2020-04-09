TRACER_DIR= ../trace-pattern-finder/PatternPlayGround/Tracer/DataFlowSanitizer/Automated
MINIZINC_FINDER_DIR= ../trace-pattern-finder/PatternPlayGround/PatternFinder/MiniZinc

include $(TRACER_DIR)/Makefile

PROCESS_TRACE= $(TRACER_DIR)/process-trace.py
MINIZINC_FLAGS = -a --solver chuffed

LOOP1_RUN = 0
LOOP1_BODY = 1
LOOP2_RUN = 2
LOOP2_BODY = 3
LOOP3_RUN = 4
LOOP3_BODY = 5
SPARSE = 6
DENSE = 7

LOOP1_SPARSE_FLAGS= --filter-marks $(LOOP1_RUN) $(SPARSE)
LOOP2_SPARSE_FLAGS= --filter-marks $(LOOP2_RUN) $(SPARSE)
LOOP3_SPARSE_FLAGS= --filter-marks $(LOOP3_RUN) $(SPARSE)

LOOP2_SPARSE_LOOP2_COLLAPSED_FLAGS= --filter-marks $(LOOP2_RUN) $(SPARSE)	\
--collapse-instances $(LOOP2_BODY):loop2-body

LOOP2_SPARSE_LOOP3_COLLAPSED_FLAGS= --filter-marks $(LOOP2_RUN) $(SPARSE)	\
--collapse-instances $(LOOP3_BODY):loop3-body

LOOP1_DENSE_FLAGS= --filter-marks $(LOOP1_RUN) $(DENSE)
LOOP2_DENSE_FLAGS= --filter-marks $(LOOP2_RUN) $(DENSE)
LOOP3_DENSE_FLAGS= --filter-marks $(LOOP3_RUN) $(DENSE)

all-pdf: smm.loop1.sparse.simple.pdf smm.loop2.sparse.simple.pdf		\
         smm.loop3.sparse.simple.pdf						\
         smm.loop2.sparse.loop2.collapsed.simple.pdf				\
         smm.loop2.sparse.loop3.collapsed.simple.pdf smm.loop1.dense.simple.pdf	\
         smm.loop2.dense.simple.pdf smm.loop3.dense.simple.pdf

all-dzn: smm.loop1.sparse.dzn smm.loop2.sparse.dzn smm.loop3.sparse.dzn	\
         smm.loop1.dense.dzn smm.loop2.dense.dzn smm.loop3.dense.dzn

all-patterns: smm.loop1.sparse.maps.pdf smm.loop2.sparse.maps.pdf	\
              smm.loop3.sparse.reductions.pdf smm.loop1.dense.maps.pdf	\
              smm.loop2.dense.maps.pdf smm.loop3.dense.reductions.pdf

%.trace: %.bin
	./$<
	mv trace $@

smm.bin: smm-instrumented.cpp
	$(CXX) $^ -o $@ -g $(CBASEFLAGS) $(CDFSANFLAGS)

smm.loop1.sparse.simple.trace: smm.simple.trace
	$(PROCESS_TRACE) transform $(LOOP1_SPARSE_FLAGS) $< > $@

smm.loop2.sparse.simple.trace: smm.simple.trace
	$(PROCESS_TRACE) transform $(LOOP2_SPARSE_FLAGS) $< > $@

smm.loop3.sparse.simple.trace: smm.simple.trace
	$(PROCESS_TRACE) transform $(LOOP3_SPARSE_FLAGS) $< > $@

smm.loop2.sparse.loop2.collapsed.simple.trace: smm.simple.trace
	$(PROCESS_TRACE) transform $(LOOP2_SPARSE_LOOP2_COLLAPSED_FLAGS) $< > $@

smm.loop2.sparse.loop3.collapsed.simple.trace: smm.simple.trace
	$(PROCESS_TRACE) transform $(LOOP2_SPARSE_LOOP3_COLLAPSED_FLAGS) $< > $@

smm.loop1.dense.simple.trace: smm.simple.trace
	$(PROCESS_TRACE) transform $(LOOP1_DENSE_FLAGS) $< > $@

smm.loop2.dense.simple.trace: smm.simple.trace
	$(PROCESS_TRACE) transform $(LOOP2_DENSE_FLAGS) $< > $@

smm.loop3.dense.simple.trace: smm.simple.trace
	$(PROCESS_TRACE) transform $(LOOP3_DENSE_FLAGS) $< > $@
