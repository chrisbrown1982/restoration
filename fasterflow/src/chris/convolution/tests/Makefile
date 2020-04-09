
CXX = g++ 

FLAGS = -O3
LFLAGS = -lOpenCL

#Non-standard OpenCL Location
OPENCLDIR = /opt/AMDAPP
OPENCLINC = -I$(OPENCLDIR)/include
LIBS = -L$(OPENCLDIR)/lib/x86_64


INCLUDE = -I.. $(OPENCLINC)


all: ffocl_test_farm ffocl_test_pipe ffocl_test_device_rule

ffocl_test_farm: HFF_device_rule_test.cpp
	$(CXX) $(FLAGS) $(INCLUDE) $(LIBS) HFF_farm_test.cpp -o ffocl_test_farm $(LFLAGS)

ffocl_test_pipe: HFF_pipe_test.cpp
	$(CXX) $(FLAGS) $(INCLUDE) $(LIBS) HFF_pipe_test.cpp -o ffocl_test_pipe $(LFLAGS)

ffocl_test_device_rule: HFF_device_rule_test.cpp
	$(CXX) $(FLAGS) $(INCLUDE) $(LIBS) HFF_device_rule_test.cpp -o ffocl_test_device_rule $(LFLAGS)

clean:
	rm ffocl_test_farm ffocl_test_pipe ffocl_test_device_rule