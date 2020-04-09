default: anisotropiefilter

anisotropiefilter: main.cpp Benchmarking.cpp Benchmarking.h Anisotropicfilter_kernel.cu PreProcessing.cu 
	  nvcc -I /home/korzeniowski/Downloads/OpenCV-2.3.1/include/opencv -I /home/korzeniowski/NVIDIA_GPU_Computing_SDK/C/common/inc -L /home/korzeniowski/NVIDIA_GPU_Computing_SDK/C/common/lib/linux main.cpp Benchmarking.cpp PreProcessing.cu Anisotropicfilter_kernel.cu -l GLEW -l opencv_core -lrt -o anisotropiefilter_gpu.out
