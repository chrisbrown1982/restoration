PROCESS_TRACE= $(TRACER_DIR)/process-trace.py

CDFSAN_COMMUTATIVITY_FLAGS = -mllvm			\
-dfsan-discovery-commutativity-list=../commutative.txt

ALL_MAPS_SZN= $(addsuffix .maps.szn, $(ALL_PREFIX))
ALL_REDUCTIONS_SZN= $(addsuffix .reductions.szn, $(ALL_PREFIX))
ALL_TRACE_PDF= $(addsuffix .trace.pdf, $(ALL_PREFIX))
ALL_MAPS_PDF= $(addsuffix .maps.pdf, $(ALL_PREFIX))
ALL_REDUCTIONS_PDF= $(addsuffix .reductions.pdf, $(ALL_PREFIX))

%.input: %.trace
	$(PROCESS_TRACE) --output-format=plain simplify $< > $@

%.trace.pdf : %.input
	$(PROCESS_TRACE) $(GRAPHVIZ_FLAGS) --output-format=graphviz print $< > $*.gv
	dot -Tpdf $*.gv > $@

%.trace: %.bin
	mkdir $<-run
	mv $< $<-run/bin
	-cd $<-run && ./bin $(BENCHMARK_ARGS)
	mv $<-run/trace $@
	rm -rf $<-run

tracing.loop%.o: ../tracing.cpp
	$(CXX) $(CXXFLAGS) -DTRACE_REGION=$(LOOP$*) $^ -c -o $@
