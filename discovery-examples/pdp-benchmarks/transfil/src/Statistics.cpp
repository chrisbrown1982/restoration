//
//  Statistics.cpp
//  transfil
//
//  Created by Paul Brown on 09/03/2017.
//  Copyright © 2017 Paul Brown. All rights reserved.
//

#include <string>
#include "Statistics.hpp"


double Statistics::gamma_dist(double k){
    
    //used to generate host bite risk . k is shape parameter, 1/k = rate parameter
    
    return gsl_ran_gamma(rando,k,1/k);
}

int Statistics::poisson_dist(double rate){
    
    //used to calculate  worm births and deaths in host
    
    if (rate<10E3)
        return gsl_ran_poisson(rando,rate);
    else
        return (int) normal_dist(rate,sqrt(rate));
    
}

double Statistics::uniform_dist(){
    
    //used to generate host age and determine when they die or import new infection and when they use a bednet
    //Also used in paramter sampling, calculating prvelance
    
    return gsl_ran_flat(rando,0,1);
}

double Statistics::normal_dist(double mu, double sigma){
    
    //sample from distribution of mean mu and SD sigma
    //used for generatring bednet usage
    
    return mu + gsl_ran_gaussian (rando, sigma);
}

double Statistics::unit_normal_dist(){
    
    // as above but sigma=1
    
    return gsl_ran_ugaussian(rando);
    
}

double Statistics::exp_dist(double mu){
    
    //exponential dist with mean mu
    return gsl_ran_exponential(rando, mu);
}


double Statistics::cdf_normal_Pinv(double p, double sd){
    
    //return value for which there is a probability p that value taken from a normal distribution with SD=1
    //will have a vaue less than or equal to it
    
    return gsl_cdf_gaussian_Pinv(p, sd);
}

std::string Statistics::selectDistribType(){
    
    //randomly select a normalor exponential distrib for random sampling at start of relicate
    if(uniform_dist() <= 0.4)
        return std::string("uniform");
    else
        return std::string("exp");
    
}



