#include "Anisotropiefilter.h"
#include "cv.h"
using namespace std; 


#define KAPPA 15
#define LAMPDA 0.25
#define OPTION 2

Anisotropiefilter::Anisotropiefilter(int imageRows, int imageCols)
{
	init(imageRows, imageCols);
}
Anisotropiefilter::~Anisotropiefilter()
{
	
}

void convertAndShow(cv::Mat& img)
{
	cv::Mat deb;
	deb.create(img.rows, img.cols, CV_8U);
	img.convertTo(deb, deb.type(), 1.0, 0.0);
        cv::namedWindow("deb", CV_WINDOW_AUTOSIZE);
	cv::imshow("deb",deb);
	cv::waitKey();

}

void Anisotropiefilter::init(int rows, int cols)
{
	this->diffl.create(rows+2, cols+2, CV_32FC1);
	this->diff.create(rows, cols, CV_32FC1);
	this->deltaN.create(rows, cols, CV_32FC1);
	this->deltaS.create(rows, cols, CV_32FC1);
	this->deltaE.create(rows, cols, CV_32FC1);
	this->deltaW.create(rows, cols, CV_32FC1);
	this->cN.create(rows, cols, CV_32FC1);
	this->cS.create(rows, cols, CV_32FC1);
	this->cE.create(rows, cols, CV_32FC1);
	this->cW.create(rows, cols, CV_32FC1);

}
inline void conductionOpt1(cv::Mat& c, cv::Mat& delta)
{
	c = delta / KAPPA;
	c = c.mul(c);
	c = -c;
	cv::exp(c, c);
	
}
inline void conductionOpt2(cv::Mat& c, cv::Mat& delta)
{
	c = delta / KAPPA;
	c = c.mul(c);
	cv::add(c, cvScalar(1), c);
	c = 1 / c;
}

inline void executeSingleImg2(const cv::Mat &diffl_roi, const cv::Mat &diff, cv::Mat &delta, cv::Mat &C)
{

	cv::subtract(diffl_roi, diff, delta);
	conductionOpt2(C, delta);
	C = C.mul(delta);

}
inline void executeSingleImg1(const cv::Mat &diffl_roi, const cv::Mat &diff, cv::Mat &delta, cv::Mat &C)
{

	cv::subtract(diffl_roi, diff, delta);
	conductionOpt1(C, delta);
	C = C.mul(delta);

}
void Anisotropiefilter::executeAnisotropiefilterMulti(const cv::Mat& image, cv::Mat& result)
{

	  
	image.convertTo(diff, diff.type(), 1, 0);

	

	cv::Mat diffl_roi(this->diffl, cv::Rect(1, 1, diff.cols, diff.rows) );
	cv::Mat diffl_roiN(this->diffl, cv::Rect(0, 1, diff.cols, diff.rows) );
	cv::Mat diffl_roiS(this->diffl, cv::Rect(2, 1, diff.cols, diff.rows) );
	cv::Mat diffl_roiE(this->diffl, cv::Rect(1, 2, diff.cols, diff.rows) );
	cv::Mat diffl_roiW(this->diffl, cv::Rect(0, 1, diff.cols, diff.rows) );

	for(int i=0; i<NITER; i++)
	{
		//construct diffl, which is the same as diff but has an extra padding of zeros around it
		diff.copyTo(diffl_roi);
		
		//create  N, S, E, W
		if(OPTION == 1)
		{
			executeSingleImg1(diffl_roiN, diff, deltaN, cN);
			executeSingleImg1(diffl_roiS, diff, deltaS, cS);
			executeSingleImg1(diffl_roiE, diff, deltaE, cE);
			executeSingleImg1(diffl_roiW, diff, deltaW, cW);
		}
		else
		{
			executeSingleImg2(diffl_roiN, diff, deltaN, cN);
			executeSingleImg2(diffl_roiS, diff, deltaS, cS);
			executeSingleImg2(diffl_roiE, diff, deltaE, cE);
			executeSingleImg2(diffl_roiW, diff, deltaW, cW);
		}
	

		cN = cN + cS + cE + cW;
		cN = cN * LAMBDA;
		diff = diff + cN;

	}

		//convertAndShow(diff);


		diff.convertTo(result, image.type(), 1, 0);

}
