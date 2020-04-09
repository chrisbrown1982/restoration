// PreProcessing : Defines the entry point for the application.
//


#include "PreProcessing.h"
#include "Anisotropiefilter.h"
#include <iostream>
#include <fstream>
#include <string>
#include "Benchmarking.h"



int main(int argc, char* argv[])
{

	//execute Anisotropiefilter 256 x 256
	cv::Mat inImage256x256(256, 256, CV_8UC1);
	cv::Mat outImage256x256(256, 256, CV_8UC1);
	generatePseudoData(inImage256x256, 256, 256);
	Anisotropiefilter anisofilter256x256(256, 256);
	
	//execute Anisotropiefilter 512 x 512
	cv::Mat inImage512x512(512, 512, CV_8UC1);
	cv::Mat outImage512x512(512, 512, CV_8UC1);
	generatePseudoData(inImage512x512, 512, 512);
	Anisotropiefilter anisofilter512x512(512, 512);
	
	//execute Anisotropiefilter 1024 x 1024
	cv::Mat inImage1024x1024(1024, 1024, CV_8UC1);
	cv::Mat outImage1024x1024(1024, 1024, CV_8UC1);
	generatePseudoData(inImage1024x1024, 1024, 1024);
	Anisotropiefilter anisofilter1024x1024(1024, 1024);
	
	//execute Anisotropiefilter 2048 x 2048
	cv::Mat inImage2048x2048(2048, 2048, CV_8UC1);
	cv::Mat outImage2048x2048(2048, 2048, CV_8UC1);
	generatePseudoData(inImage2048x2048, 2048, 2048);
	Anisotropiefilter anisofilter2048x2048(2048, 2048);


	//execute Anisotropiefilter 4096 x 4096
	cv::Mat inImage4096x4096(4096, 4096, CV_8UC1);
	cv::Mat outImage4096x4096(4096, 4096, CV_8UC1);
	generatePseudoData(inImage4096x4096, 4096, 4096);
	Anisotropiefilter anisofilter4096x4096(4096, 4096);


	timespec time1, time2;
	timespec tp1,tp2; 
	double startTime;
	double stopTime;


	

	//measure time begin
	Benchmarking benchmark;
	/*int b1 = benchmark.getInterval("Anisotropic_256x256");
	int b2 = benchmark.getInterval("Anisotropic_512x512");
	int b3 = benchmark.getInterval("Anisotropic_1024x1024");
	int b4 = benchmark.getInterval("Anisotropic_2048x2048");
	int b5 = benchmark.getInterval("Anisotropic_4096x4096");
	*/

	cout << "number of iterations: " << NITER << endl;



  
	clock_gettime(CLOCK_REALTIME,&tp1);
  	startTime = (double)(tp1.tv_sec)*1.0e9 + (double)(tp1.tv_nsec);   
  

  	clock_gettime(CLOCK_REALTIME,&tp2);
  	stopTime = (double)(tp2.tv_sec)*1.0e9 + (double)(tp2.tv_nsec);   
  	std::cout<<"exclapsed time: "<<(stopTime-startTime)*1.e-9<<std::endl; 

	//##########################################################
	/*
	clock_gettime(CLOCK_REALTIME, &time1);

	//start calculation
	sleep(10);

	clock_gettime(CLOCK_REALTIME, &time2);
	cout << "test 10 seconds: " << benchmark.diff(time1, time2).tv_sec << ":" << benchmark.diff(time1,time2).tv_nsec/1000000<< " ms" << endl;
*/
	//##########################################################
	
	clock_gettime(CLOCK_REALTIME,&tp1);
  	startTime = (double)(tp1.tv_sec)*1.0e9 + (double)(tp1.tv_nsec);

	//start calculation
	anisofilter256x256.executeAnisotropiefilterMulti(inImage256x256, outImage256x256);

	clock_gettime(CLOCK_REALTIME,&tp2);
  	stopTime = (double)(tp2.tv_sec)*1.0e9 + (double)(tp2.tv_nsec);   
  	cout<<"exclapsed time 256x256: "<<(stopTime-startTime)*1.e-9<<std::endl;

	//##########################################################
	clock_gettime(CLOCK_REALTIME,&tp1);
  	startTime = (double)(tp1.tv_sec)*1.0e9 + (double)(tp1.tv_nsec);

	//start calculation
	anisofilter512x512.executeAnisotropiefilterMulti(inImage512x512, outImage512x512);

	clock_gettime(CLOCK_REALTIME,&tp2);
  	stopTime = (double)(tp2.tv_sec)*1.0e9 + (double)(tp2.tv_nsec);   
  	std::cout<<"exclapsed time 512x512: "<<(stopTime-startTime)*1.e-9<<std::endl;

	//##########################################################
	clock_gettime(CLOCK_REALTIME,&tp1);
  	startTime = (double)(tp1.tv_sec)*1.0e9 + (double)(tp1.tv_nsec);

	//start calculation
	anisofilter1024x1024.executeAnisotropiefilterMulti(inImage1024x1024, outImage1024x1024);

	clock_gettime(CLOCK_REALTIME,&tp2);
  	stopTime = (double)(tp2.tv_sec)*1.0e9 + (double)(tp2.tv_nsec);   
  	std::cout<<"exclapsed time 1024x1024: "<<(stopTime-startTime)*1.e-9<<std::endl;

	//##########################################################
	clock_gettime(CLOCK_REALTIME,&tp1);
  	startTime = (double)(tp1.tv_sec)*1.0e9 + (double)(tp1.tv_nsec);

	//start calculation
	anisofilter2048x2048.executeAnisotropiefilterMulti(inImage2048x2048, outImage2048x2048);

	clock_gettime(CLOCK_REALTIME,&tp2);
  	stopTime = (double)(tp2.tv_sec)*1.0e9 + (double)(tp2.tv_nsec);   
  	std::cout<<"exclapsed time 2048x2048: "<<(stopTime-startTime)*1.e-9<<std::endl;

	//##########################################################
	clock_gettime(CLOCK_REALTIME,&tp1);
  	startTime = (double)(tp1.tv_sec)*1.0e9 + (double)(tp1.tv_nsec);

	//start calculation
	anisofilter4096x4096.executeAnisotropiefilterMulti(inImage4096x4096, outImage4096x4096);

	clock_gettime(CLOCK_REALTIME,&tp2);
  	stopTime = (double)(tp2.tv_sec)*1.0e9 + (double)(tp2.tv_nsec);   
  	std::cout<<"exclapsed time 4096x4096: "<<(stopTime-startTime)*1.e-9<<std::endl;

/*

	benchmark.start(b1);
	
	//start calculation
	anisofilter256x256.executeAnisotropiefilterMulti(inImage256x256, outImage256x256);

	//measure time end
	benchmark.finish(b1);

	benchmark.start(b2);
	
	//start calculation
	anisofilter512x512.executeAnisotropiefilterMulti(inImage512x512, outImage512x512);

	//measure time end
	benchmark.finish(b2);

	benchmark.start(b3);
	
	//start calculation
	anisofilter1024x1024.executeAnisotropiefilterMulti(inImage1024x1024, outImage1024x1024);

	//measure time end
	benchmark.finish(b3);

	benchmark.start(b4);
	
	//start calculation
	anisofilter2048x2048.executeAnisotropiefilterMulti(inImage2048x2048, outImage2048x2048);

	//measure time end
	benchmark.finish(b4);

	benchmark.start(b5);
	
	//start calculation
	anisofilter4096x4096.executeAnisotropiefilterMulti(inImage4096x4096, outImage4096x4096);

	//measure time end
	benchmark.finish(b5);
	
	//std::cout<<"Execution Time : " << benchmark.getTime(b1) <<" ms." << std::endl;
	benchmark.printresult();*/

return 0;

}

void generatePseudoData(cv::Mat src, int width, int height)
{

	for(int j=0; j<height; j++)
	{
		for(int i=0; i<width; i++)
		{
			src.at<uchar>(j,i) = 1 + (i+j)/255;
		}
	}

}
