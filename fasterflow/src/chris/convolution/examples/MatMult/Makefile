XX = g++

FLAGS = -O3
LFLAGS = -lpng12 -lOpenCL

#Non-standard OpenCL Location
OPENCLINC = /usr/local/cuda/include
PNGINC = /usr/include/libpng12
LIBS = -L/usr/lib64


INCLUDE = -I../.. -I$(OPENCLINC) -I$(PNGINC)


all: ffocl_nested_farm

ffocl_nested_farm: 
	$(CXX) $(FLAGS) $(INCLUDE) cpu2.cpp -o cpu2 $(LFLAGS)

clean:
	rm ffocl_nested_farm
