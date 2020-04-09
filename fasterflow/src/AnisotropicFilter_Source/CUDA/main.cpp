/*
*
*/

// includes, system
#include <iostream>
#include <stdlib.h>
#include <iostream>

// Required to include CUDA vector types
//#include <vector_types.h>
//#include "cutil_inline.h"
#include <cv.h>
#include <highgui.h>
#include "Benchmarking.h"

////////////////////////////////////////////////////////////////////////////////
// declaration, forward
extern "C" void executePreProcessing(uchar* data, uchar* output, int len);
extern "C" void initPreProcessing(int imWidth, int imHeight, int niter, double kappa, double lampda, int type);


////////////////////////////////////////////////////////////////////////////////
// Program main
////////////////////////////////////////////////////////////////////////////////

void generatePseudoData(cv::Mat src, int width, int height)
{

	for(int j=0; j<height; j++)
	{
		for(int i=0; i<width; i++)
		{
			src.at<float>(j,i) = 1 + (i+j)/255;
		}
	}

}
int main(int argc, char** argv)
{	
	int niter = 10;
	float kappa = 5.0f;
	float lampda = 0.25f;
	int type = 2;
	timespec tp1,tp2; 
	double startTime;
	double stopTime;

	//####################execute Anisotropiefilter 256 x 256#############################
	int width = 256;
	int height = 256;
	cv::Mat inImage256x256(height, width, CV_32FC1);
	cv::Mat outImage256x256(height, width, CV_32FC1);
	generatePseudoData(inImage256x256, height, width);
	
	//init values on graphiccard and warmup
	initPreProcessing(width, height, niter, kappa, lampda,type);

	//measure time begin
	Benchmarking benchmark;
	//int b1 = benchmark.getInterval("GPU-Anisotropicfilter256x256");


	//benchmark.start(b1);
    	// run the device part of the program

	clock_gettime(CLOCK_REALTIME,&tp1);
  	startTime = (double)(tp1.tv_sec)*1.0e9 + (double)(tp1.tv_nsec);   


	executePreProcessing(inImage256x256.data, outImage256x256.data, width*height);

	clock_gettime(CLOCK_REALTIME,&tp2);
  	stopTime = (double)(tp2.tv_sec)*1.0e9 + (double)(tp2.tv_nsec);   
  	std::cout<<"exclapsed time 256x256: "<<(stopTime-startTime)*1.e-9<<std::endl; 

	

	//measure time end
	//benchmark.finish(b1);

	//#########################execute Anisotropiefilter 512 x 512##################################
	width = 512;
	height = 512;
	cv::Mat inImage512x512(height, width, CV_32FC1);
	cv::Mat outImage512x512(height, width, CV_32FC1);
	generatePseudoData(inImage512x512, height, width);
	
	//init values on graphiccard and warmup
	initPreProcessing(width, height, niter, kappa, lampda,type);

	//measure time begin
	//int b2 = benchmark.getInterval("GPU-Anisotropicfilter512x512");


	//benchmark.start(b2);

	clock_gettime(CLOCK_REALTIME,&tp1);
  	startTime = (double)(tp1.tv_sec)*1.0e9 + (double)(tp1.tv_nsec);

    // run the device part of the program
	executePreProcessing(inImage512x512.data, outImage512x512.data, width*height);


	clock_gettime(CLOCK_REALTIME,&tp2);
  	stopTime = (double)(tp2.tv_sec)*1.0e9 + (double)(tp2.tv_nsec);   
  	std::cout<<"exclapsed time 512x512: "<<(stopTime-startTime)*1.e-9<<std::endl; 
	

	//measure time end
	//benchmark.finish(b2);

	//########################execute Anisotropiefilter 1024 x 1024#################################
	width = 1024;
	height = 1024;
	cv::Mat inImage1024x1024(height, width, CV_32FC1);
	cv::Mat outImage1024x1024(height, width, CV_32FC1);
	generatePseudoData(inImage1024x1024, height, width);
	
	//init values on graphiccard and warmup
	initPreProcessing(width, height, niter, kappa, lampda,type);

	//measure time begin
	//int b3 = benchmark.getInterval("GPU-Anisotropicfilter1024x1024");

	clock_gettime(CLOCK_REALTIME,&tp1);
  	startTime = (double)(tp1.tv_sec)*1.0e9 + (double)(tp1.tv_nsec);

	//benchmark.start(b3);
    // run the device part of the program
	executePreProcessing(inImage1024x1024.data, outImage1024x1024.data, width*height);

	clock_gettime(CLOCK_REALTIME,&tp2);
  	stopTime = (double)(tp2.tv_sec)*1.0e9 + (double)(tp2.tv_nsec);   
  	std::cout<<"exclapsed time 1024x1024: "<<(stopTime-startTime)*1.e-9<<std::endl; 

	//measure time end
	//benchmark.finish(b3);

	//########################execute Anisotropiefilter 2048 x 2048####################################
	width = 2048;
	height = 2048;
	cv::Mat inImage2048x2048(height, width, CV_32FC1);
	cv::Mat outImage2048x2048(height, width, CV_32FC1);
	generatePseudoData(inImage2048x2048, height, width);
	
	//init values on graphiccard and warmup
	initPreProcessing(width, height, niter, kappa, lampda,type);

	//measure time begin
	//int b4 = benchmark.getInterval("GPU-Anisotropicfilter2048x2048");

	clock_gettime(CLOCK_REALTIME,&tp1);
  	startTime = (double)(tp1.tv_sec)*1.0e9 + (double)(tp1.tv_nsec);

	//benchmark.start(b4);
    // run the device part of the program
	executePreProcessing(inImage2048x2048.data, outImage2048x2048.data, width*height);

	clock_gettime(CLOCK_REALTIME,&tp2);
  	stopTime = (double)(tp2.tv_sec)*1.0e9 + (double)(tp2.tv_nsec);   
  	std::cout<<"exclapsed time 2048x2048: "<<(stopTime-startTime)*1.e-9<<std::endl; 

	//measure time end
	//benchmark.finish(b4);


	//########################execute Anisotropiefilter 4096 x 4096####################################
	width = 4096;
	height = 4096;
	cv::Mat inImage4096x4096(height, width, CV_32FC1);
	cv::Mat outImage4096x4096(height, width, CV_32FC1);
	generatePseudoData(inImage4096x4096, height, width);
	
	//init values on graphiccard and warmup
	initPreProcessing(width, height, niter, kappa, lampda,type);

	//measure time begin
	//int b5 = benchmark.getInterval("GPU-Anisotropicfilter4096x4096");

	clock_gettime(CLOCK_REALTIME,&tp1);
  	startTime = (double)(tp1.tv_sec)*1.0e9 + (double)(tp1.tv_nsec);

	//benchmark.start(b5);
    // run the device part of the program
	executePreProcessing(inImage4096x4096.data, outImage4096x4096.data, width*height);

	clock_gettime(CLOCK_REALTIME,&tp2);
  	stopTime = (double)(tp2.tv_sec)*1.0e9 + (double)(tp2.tv_nsec);   
  	std::cout<<"exclapsed time 4096x4096: "<<(stopTime-startTime)*1.e-9<<std::endl; 
	

	//measure time end
	//benchmark.finish(b5);

	//benchmark.printresult();

	//cutilExit(argc, argv);

    

}
