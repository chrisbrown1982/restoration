//
//  Statistics.hpp
//  transfil
//
//  Created by Paul Brown on 09/03/2017.
//  Copyright © 2017 Paul Brown. All rights reserved.
//

#ifndef Statistics_hpp
#define Statistics_hpp

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>
//multivariate normal libraries
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include <cmath>
#include <chrono>
#include <algorithm>


//Container class for gsl random number functions required by many other classes

class Statistics {
    
public:
    
    Statistics(){
        
        gsl_rng_env_setup();
        rando = gsl_rng_alloc(gsl_rng_default); //rando is a pointer to the random number generator
        gsl_rng_set(rando,0); //set seed
        
    }
    
    ~Statistics(){
        gsl_rng_free(rando);
    };
    
    double gamma_dist(double k);
    double normal_dist(double mean, double sd);
    double unit_normal_dist();
    double uniform_dist();
    int poisson_dist(double rate);
    double exp_dist(double mu);
    double cdf_normal_Pinv(double p, double sd);
    
    std::string selectDistribType();
   
    
    
private:
    
    gsl_rng * rando;
    
};



#endif /* Statistics_hpp */

