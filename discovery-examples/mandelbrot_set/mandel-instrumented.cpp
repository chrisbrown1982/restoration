#include <iostream>
#include <complex>
#include <vector>
#include <chrono>
#include <functional>
#include <sanitizer/dfsan_interface.h>


#include "window.h"
#include "save_image.h"
#include "utils.h"

#define LOOP1_BODY 1
#define LOOP2_BODY 2
#define LOOP3_BODY 3
#define SCALE_BODY 4
#define ESCAPE_BODY 5
#define FUNC_BODY 6

bool loop1_marked = false;
bool loop2_marked = false;
bool loop3_marked = false;

//#include "tbb/tbb.h"
///#include "tbb/pipeline.h"

// clang++ -std=c++11 -stdlib=libc++ -O3 save_image.cpp utils.cpp mandel.cpp -lfreeimage

// Use an alias to simplify the use of complex type
using Complex = std::complex<double>;


// Convert a pixel coordinate to the complex domain
Complex scale(window<int> &scr, window<double> &fr, Complex c) {
        dfsan_begin_marking(SCALE_BODY);
	Complex aux(c.real() / (double)scr.width() * fr.width() + fr.x_min(),
		c.imag() / (double)scr.height() * fr.height() + fr.y_min());
        dfsan_end_marking(SCALE_BODY);
	return aux;
}

// Check if a point is in the set or escapes to infinity, return the number if iterations
int escape(Complex c, int iter_max, const std::function<Complex( Complex, Complex)> &func) {
        dfsan_begin_marking(ESCAPE_BODY);
	Complex z(0,0);
	int iter = 0;

        // Hot loop, level 3, takes 90% of runtime.
        if (!loop3_marked) {
          dfsan_begin_marking(LOOP3_BODY);
          loop3_marked = true;
        }
	while (abs(z) < 2.0 && iter < iter_max) {
		z = func(z, c);
		iter++;
	}
        if (loop3_marked) {
          dfsan_end_marking(LOOP3_BODY);
        }
        dfsan_end_marking(ESCAPE_BODY);
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

void get_number_iterations(window<int> &scr, window<double> &fract, int iter_max, std::vector<int> &colors,
		const std::function<Complex(Complex, Complex)> &func) {
	int k = 0;
	int N = scr.width();
        // Hot loop, level 1, takes 96% of runtime.
        dfsan_begin_marking(LOOP1_BODY);
	for(int i = scr.y_min(); i < scr.y_max(); ++i) {
		// int Ni = N*i;
		// Hot loop, level 2, takes 96% of runtime.
                if (!loop2_marked) {
                  dfsan_begin_marking(LOOP2_BODY);
                  loop2_marked = true;
                }
		for(int j = scr.x_min(); j < scr.x_max(); ++j) {
			Complex c((double) (j), (double) (i));
			c = scale(scr, fract, c);
			colors[k] = escape(c, iter_max, func);  // Replace with Ni+j
			k += 1;
		}
                if (loop2_marked) {
                  dfsan_end_marking(LOOP2_BODY);
                }
	}
        dfsan_end_marking(LOOP1_BODY);
}

void fractal(window<int> &scr, window<double> &fract, int iter_max, std::vector<int> &colors,
	const std::function<Complex( Complex, Complex)> &func, const char *fname, bool smooth_color) {
	auto start = std::chrono::steady_clock::now();
	get_number_iterations(scr, fract, iter_max, colors, func);
	auto end = std::chrono::steady_clock::now();
	std::cout << "Time to generate " << fname << " = " << std::chrono::duration <double, std::milli> (end - start).count() << " [ms]" << std::endl;

	// Save (show) the result as an image
	plot(scr, colors, iter_max, fname, smooth_color);
}

void mandelbrot() {
	// Define the size of the image
	window<int> scr(0, 1200, 0, 1200);
	// The domain in which we test for points
	window<double> fract(-2.2, 1.2, -1.7, 1.7);

	// The function used to calculate the fractal
	auto func = [] (Complex z, Complex c) -> Complex {return z * z + c; };

	int iter_max = 500;
	const char *fname = "mandelbrot.png";
	bool smooth_color = true;
	std::vector<int> colors(scr.size());

	// Experimental zoom (bugs ?). This will modify the fract window (the domain in which we calculate the fractal function) 
	zoom(1.0, -1.225, -1.22, 0.15, 0.16, fract); //Z2
	
	fractal(scr, fract, iter_max, colors, func, fname, smooth_color);
}

void small_mandelbrot() {
	// Define the size of the image
	window<int> scr(0, 4, 0, 4);
	// The domain in which we test for points
	window<double> fract(-0.1, 0.1, -0.1, 0.1);
	// The function used to calculate the fractal
	auto func = [] (Complex z, Complex c) -> Complex {
          dfsan_begin_marking(FUNC_BODY);
          Complex r = z * z + c;
          dfsan_end_marking(FUNC_BODY);
          return r; };
	int iter_max = 4;
	const char *fname = "mandelbrot.png";
	bool smooth_color = true;
	std::vector<int> colors(scr.size());
	fractal(scr, fract, iter_max, colors, func, fname, smooth_color);
}

void triple_mandelbrot() {
	// Define the size of the image
	window<int> scr(0, 1200, 0, 1200);
	// The domain in which we test for points
	window<double> fract(-1.5, 1.5, -1.5, 1.5);

	// The function used to calculate the fractal
	auto func = [] (Complex z, Complex c) -> Complex {return z * z * z + c; };

	int iter_max = 500;
	const char *fname = "triple_mandelbrot.png";
	bool smooth_color = true;
	std::vector<int> colors(scr.size());

	fractal(scr, fract, iter_max, colors, func, fname, smooth_color);
}

int main() {
  small_mandelbrot();
//	mandelbrot();
//	triple_mandelbrot();

	return 0;
}
