/************************************************************************
  			Anisotropiefilter.h
  			
% ANISODIFF - Anisotropic diffusion.
%
% Usage:
%  diff = anisodiff(im, niter, kappa, lambda, option)
%
% Arguments:
%         im     - input image
%         niter  - number of iterations.
%         kappa  - conduction coefficient 20-100 ?
%         lambda - max value of .25 for stability
%         option - 1 Perona Malik diffusion equation No 1
%                  2 Perona Malik diffusion equation No 2
%
% Returns:
%         diff   - diffused image.
%
% kappa controls conduction as a function of gradient.  If kappa is low
% small intensity gradients are able to block conduction and hence diffusion
% across step edges.  A large value reduces the influence of intensity
% gradients on conduction.
%
% lambda controls speed of diffusion (you usually want it at a maximum of
% 0.25)
%
% Diffusion equation 1 favours high contrast edges over low contrast ones.
% Diffusion equation 2 favours wide regions over smaller ones.

% Reference: 
% P. Perona and J. Malik. 
% Scale-space and edge detection using ansotropic diffusion.
% IEEE Transactions on Pattern Analysis and Machine Intelligence, 
% 12(7):629-639, July 1990.
%
% Peter Kovesi  
% School of Computer Science & Software Engineering
% The University of Western Australia
% pk @ csse uwa edu au
% http://www.csse.uwa.edu.au
%
% June 2000  original version.       
% March 2002 corrected diffusion eqn No 2.

**************************************************************************/

	

#include "highgui.h"


#define NITER 10


class Anisotropiefilter
{


public:
	Anisotropiefilter(int imageRows, int imageCols);
	~Anisotropiefilter();
	void executeAnisotropiefilterMulti(const cv::Mat& image, cv::Mat& result);

private:
	void init(int imageRows, int imageCols);
	void convertAndShow(cv::Mat& img);
	cv::Mat diffl;
	cv::Mat diff;
	cv::Mat deltaN;
	cv::Mat deltaS;
	cv::Mat deltaE;
	cv::Mat deltaW;
	cv::Mat cN;
	cv::Mat cS;
	cv::Mat cE;
	cv::Mat cW;

	int KAPPA; 
	double LAMBDA; 
	int OPTION;

};


