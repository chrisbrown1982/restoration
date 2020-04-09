//
//  Host.cpp
//  transfil
//
//  Created by Paul Brown on 27/01/2017.
//  Copyright © 2017 Paul Brown. All rights reserved.
//

#include <climits>
#include "Host.hpp"
#include "Statistics.hpp"
#include "Worm.hpp"
#include "Vector.hpp"


extern Statistics stats;


void Host::reset(int a){
    
    
    //called when host dies and is reborn or initialised
    
    WM = WF = 0;
    M = 0.0;
    bedNet = 0;
    monthsSinceTreated = UINT_MAX; // never treated;
    age = a;
    
}


void Host::initialise(double deathRate, int maxAge, double k, double* totalBiteRisk){
    
    //called at start of new replicate
    
    //set random age
    int a = (-1 / deathRate)* log(1- stats.uniform_dist() *(1 - exp(-deathRate * maxAge))) * 12.0;
    //set infection risk
    *totalBiteRisk += (biteRisk = stats.gamma_dist(k)); //static var records totla risk of whole population
    //bite risk is mean bites per month
    
    reset(a);
    
}


                               

void Host::react( double dt, double deathRate, const int maxAge, double aImp, const Vector& vectors, const Worm& worms ) {
    
    //time-step
    age += dt;
    
    //each month 3 possible fates
    
    if ((hostDies(deathRate * dt)) || age > (12*maxAge)){ //if over age 100
        
        //host dies and is replaced by uninfected newborn with same bite risk (b)
        reset(0);
        
    }else{
        //host lives on
        
        double biteRateScaleFactor = (age < 108.0)? age/108.0:1.0; //increases with age up to 9 years. If < 9, scale downwards to account for smaller surface area
        
        if (newImportation(aImp * dt)){
        
            //host leaves and is replaced by another individual of same age (a) and bite risk infected with a breeding pair of worms but no microfilarae
        
            //import a new individual already infected with a breeding pair of worms set at their expectation with L3 set to 10 arbitrarily.
            //xi = bite rate* num mosquitos * prob of being infected after being bitten
            
            //xi = Vector.lambda * Vector.v_to_h * Worm.psi1 * Worm.psi2 * Worm.s2
            //lbda - model.l3, model.bednet, vh - nowhere else, psi1,2 - model.l3, s2 - nowhere
            
            //numer time bitten * prop of larval density (10) in vector that will become adult worms
            WM = WF = (int) (0.5 * vectors.averageNumBites() * biteRateScaleFactor * 10 * worms.proportionPerBite() / worms.getDeathRate()); //prob of being bitten and infected from a bite/worm death rate (default values make ~16)
            M = 0;
        
        }else{
        
            //worm load is updated
        
            double bites = vectors.averageNumBites() * biteRisk * biteRateScaleFactor; //average bite rate scaled to this individual and further scaled down if they are under 9 years old
            if (bedNet) bites *= vectors.probBitesThroughNet(); // reduce bites if host has bednet
            
            double wormsPerBite = vectors.getL3Density() * worms.proportionPerBite(); //density of larvae in vector poln * proportion entering host and growing up to adult worms
            double meanWorms = 0.5 * bites * wormsPerBite; //This is mean per unit time of poisson distribution. 0.5 as half of each gender
            
            int births, deaths;
            
            //male worm update
            births = stats.poisson_dist(meanWorms * dt);
            deaths = stats.poisson_dist( worms.getDeathRate()  * (double) WM * dt );
            WM += (births - deaths);
            
            //female worm update
            births = stats.poisson_dist(meanWorms * dt); //* exp(-1 * beta * I)
            deaths = stats.poisson_dist( worms.getDeathRate() * (double) WF * dt );
            WF += (births - deaths);
            
            //Mf update
            double mdeaths = dt * worms.getMFDeathRate() * M;// time * death rate * number
            double mbirths = dt * worms.repRate(monthsSinceTreated, WF, WM);
            M += (mbirths - mdeaths);
  
            
        }
        //drugs wear off each month
        monthsSinceTreated = (monthsSinceTreated+dt < UINT_MAX)? monthsSinceTreated+dt : UINT_MAX;
    
        //ensure all positive state variables remain positive

        WM = (WM < 0)? 0:WM;
        WF = (WF < 0)? 0:WF;
        M = (M < 0.0)? 0.0:M;
        
    }
    
}

bool Host::newImportation(double prob){
    
    return stats.uniform_dist() < (1- exp(-prob));
}

bool Host::hostDies(double prob){
    
    return stats.uniform_dist() < (1 - exp(-prob));
}


void Host::getsTreated(Worm& worms, std::string type ){
    
    //MDA kill portion of worms
    
    M = worms.mfTreated(M, type);
    WM = worms.wormsTreated(WM, type);
    WF = worms.wormsTreated(WF, type);
    
    monthsSinceTreated = 0;
    
}

Host::operator hostState() const{
    
    //save this object to the host state structure
    return {WM, WF, M, biteRisk, age, monthsSinceTreated};
    
    
}

void Host::restore(const hostState& state){
    
    //restore form hoststate object
    WM = state.WM;
    WF = state.WF;
    M = state.M;
    biteRisk = state.biteRisk;
    age = state.age;
    monthsSinceTreated = state.monthsSinceTreated;
    
}
    






















