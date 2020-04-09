TRACER_DIR= ../trace-pattern-finder/PatternPlayGround/Tracer/DataFlowSanitizer/Automated
MINIZINC_FINDER_DIR= ../trace-pattern-finder/PatternPlayGround/PatternFinder/MiniZinc

include $(TRACER_DIR)/Makefile

PROCESS_TRACE= $(TRACER_DIR)/process-trace.py
MINIZINC_FLAGS = -a --solver chuffed

LOOP1_FLAGS= --filter-mark 1 --collapse-instances 4:scale 5:escape
LOOP2_FLAGS= --filter-mark 2 --collapse-instances 4:scale 5:escape
LOOP3_FLAGS= --filter-mark 3 --collapse-instances 6:func

GRAPHVIZ_FLAGS= --simplify-loc --print-basefile-loc --output-format=graphviz

all-pdf: mandelbrot.loop1.simple.pdf mandelbrot.loop2.simple.pdf mandelbrot.loop3.simple.pdf

all-dzn: mandelbrot.loop1.dzn mandelbrot.loop2.dzn mandelbrot.loop3.dzn

all-patterns: mandelbrot.loop1.maps.pdf mandelbrot.loop2.maps.pdf

%.trace: %.bin
	./$<
	mv trace $@

mandelbrot.bin: mandel-instrumented.cpp save_image.cpp utils.cpp
	$(CXX) $^ -o $@ -g -std=c++1y $(CBASEFLAGS) $(CDFSANFLAGS) -lfreeimage

mandelbrot.loop1.simple.trace: mandelbrot.simple.trace
	$(PROCESS_TRACE) transform $(LOOP1_FLAGS) $< > $@

mandelbrot.loop2.simple.trace: mandelbrot.simple.trace
	$(PROCESS_TRACE) transform $(LOOP2_FLAGS) $< > $@

mandelbrot.loop3.simple.trace: mandelbrot.simple.trace
	$(PROCESS_TRACE) transform $(LOOP3_FLAGS) $< > $@

%.trace.gv: %.trace
	$(PROCESS_TRACE) $(GRAPHVIZ_FLAGS) print $< > $@
