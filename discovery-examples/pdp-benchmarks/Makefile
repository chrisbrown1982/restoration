BENCHMARKS= mandelbrot ant-colony convolution blackscholes transfil

all-maps-szn: $(addsuffix -all-maps-szn, $(BENCHMARKS))
all-reductions-szn: $(addsuffix -all-reductions-szn, $(BENCHMARKS))
all-patterns-szn: $(addsuffix -all-patterns-szn, $(BENCHMARKS))
all-trace-pdf: $(addsuffix -all-trace-pdf, $(BENCHMARKS))
all-maps-pdf: $(addsuffix -all-maps-pdf, $(BENCHMARKS))
all-reductions-pdf: $(addsuffix -all-reductions-pdf, $(BENCHMARKS))
clean: $(addsuffix -clean, $(BENCHMARKS))

%-all-maps-szn:
	$(MAKE) -C $* all-maps-szn
	cp $*/*.maps.szn .

%-all-reductions-szn:
	$(MAKE) -C $* all-reductions-szn
	cp $*/*.reductions.szn .

%-all-patterns-szn:
	$(MAKE) -C $* all-maps-szn all-reductions-szn
	cp $*/*.szn .

%-all-trace-pdf:
	$(MAKE) -C $* all-trace-pdf
	cp $*/*.trace.pdf .

%-all-maps-pdf:
	$(MAKE) -C $* all-maps-pdf
	cp $*/*.maps.pdf .

%-all-reductions-pdf:
	$(MAKE) -C $* all-reductions-pdf
	cp $*/*.reductions.pdf .

%-clean:
	$(MAKE) -C $* clean

patterns.csv: reference/expert-patterns.csv all-patterns-szn
	./process-results.py reference/expert-patterns.csv . > patterns.csv
