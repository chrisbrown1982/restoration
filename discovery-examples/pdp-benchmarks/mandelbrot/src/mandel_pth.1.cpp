#include <iostream>
#include <complex>
#include <vector>
#include <chrono>
#include <functional>


#include "window.h"
#include "save_image.h"
#include "utils.h"

//#include "tbb/tbb.h"
///#include "tbb/pipeline.h"

// #include <pthread.h>

#define YMAX 1200
#define MAXWORKERS YMAX

// pthread_t workers[MAXWORKERS];

// clang++ -std=c++11 -stdlib=libc++ -O3 save_image.cpp utils.cpp mandel.cpp -lfreeimage

// Use an alias to simplify the use of complex type
using Complex = std::complex<double>;

window<int> scr(0, YMAX, 0, YMAX);
window<double> fract(-2.2, 1.2, -1.7, 1.7);
int iter_max;
std::vector<int> colors(scr.size());
const std::function<Complex(Complex, Complex)> func = [] (Complex z, Complex c) -> Complex {return z * z + c; };


// Convert a pixel coordinate to the complex domain
Complex scale(window<int> &scr, window<double> &fr, Complex c) {
	Complex aux(c.real() / (double)scr.width() * fr.width() + fr.x_min(),
		c.imag() / (double)scr.height() * fr.height() + fr.y_min());
	return aux;
}

// Check if a point is in the set or escapes to infinity, return the number if iterations
// int escape(Complex c, int iter_max, const std::function<Complex( Complex, Complex)> &func) {
int escape(Complex c) {
	Complex z(0,0);
	int iter = 0;

	while (abs(z) < 2.0 && iter < iter_max) {
		z = func(z, c);
		iter++;
	}

	return iter;
}

// Loop over each pixel from our image and check if the points associated with this pixel escape to infinity
/*
 void get_number_iterations(window<int> &scr, window<double> &fract, int iter_max, std::vector<int> &colors,
	const std::function<Complex(Complex, Complex)> &func) {
	int k = 0;
	int N = scr.width();
	for(int i = scr.y_min(); i < scr.y_max(); ++i) {
		for(int j = scr.x_min(); j < scr.x_max(); ++j) {
			Complex c((double) (j), (double) (i));
			c = scale(scr, fract, c);
			colors[N * i + j] = escape(c, iter_max, func);
		}
	}
}*/

// void worker(int i) {
void * worker(void * arg) {
	long i = (long) arg;
	for(int j = scr.x_min(); j < scr.x_max(); ++j) {
		Complex c((double) (j), (double) (i));
		c = scale(scr, fract, c);
		colors[(scr.width()*i) + j] = escape(c);
	}
}

void get_number_iterations() {
	// int N = scr.width();
	for(long i = scr.y_min(); i < scr.y_max(); ++i) {
		// pthread_create(&workers[i], NULL, worker, (void *) i);
		worker((void *) i);
	}
	for (long i = scr.y_min(); i < scr.y_max(); ++i) {
		// pthread_join(workers[i], NULL);
	}
}

void fractal(const char *fname, bool smooth_color) {
	auto start = std::chrono::steady_clock::now();
	// get_number_iterations(scr, fract, iter_max, colors, func);
	get_number_iterations();
	auto end = std::chrono::steady_clock::now();
	std::cout << "Time to generate " << fname << " = " << std::chrono::duration <double, std::milli> (end - start).count() << " [ms]" << std::endl;

	// Save (show) the result as an image
	plot(scr, colors, iter_max, fname, smooth_color);
}

void mandelbrot() {
	// Define the size of the image
	// window<int> scr(0, 1200, 0, 1200);
	// The domain in which we test for points
	// window<double> fract(-2.2, 1.2, -1.7, 1.7);

	// The function used to calculate the fractal
	// auto f = 

	iter_max = 500;
	const char *fname = "mandelbrot.png";
	bool smooth_color = true;
	// std::vector<int> colors(scr.size());

	// Experimental zoom (bugs ?). This will modify the fract window (the domain in which we calculate the fractal function) 
	zoom(1.0, -1.225, -1.22, 0.15, 0.16, fract); //Z2
	
	fractal(fname, smooth_color);
}

// void triple_mandelbrot() {
// 	// Define the size of the image
// 	window<int> scr(0, 1200, 0, 1200);
// 	// The domain in which we test for points
// 	window<double> fract(-1.5, 1.5, -1.5, 1.5);

// 	// The function used to calculate the fractal
// 	auto func = [] (Complex z, Complex c) -> Complex {return z * z * z + c; };

// 	int iter_max = 500;
// 	const char *fname = "triple_mandelbrot.png";
// 	bool smooth_color = true;
// 	std::vector<int> colors(scr.size());

// 	fractal(scr, fract, iter_max, colors, func, fname, smooth_color);
// }

int main() {
	mandelbrot();
//	triple_mandelbrot();

	return 0;
}
