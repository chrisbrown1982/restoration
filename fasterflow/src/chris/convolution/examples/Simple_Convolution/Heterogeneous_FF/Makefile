
CXX = g++ 

FLAGS = -O3
LFLAGS = -lpng12 -lOpenCL

#Non-standard OpenCL Location
OPENCLINC = /usr/local/cuda/include
PNGINC = /usr/include/libpng12
LIBS = -L/usr/lib64


INCLUDE = -I../../.. -I$(OPENCLINC) -I$(PNGINC)


all: ffocl_nested_farm 

ffocl_nested_farm: 
	$(CXX) $(FLAGS) $(INCLUDE) farm_mix_cpu_gpu.cpp -o ffocl_nested_farm $(LFLAGS)

clean:
	rm ffocl_nested_farm
