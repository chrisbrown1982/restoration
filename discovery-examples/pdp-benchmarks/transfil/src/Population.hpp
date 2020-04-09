//
//  Population.hpp
//  transfil
//
//  Created by Paul Brown on 03/03/2017.
//  Copyright © 2017 Paul Brown. All rights reserved.
//

#ifndef Population_hpp
#define Population_hpp

#include <iostream>
#include <string>
#include <random>
#include <map>
#include "Host.hpp"
#include "Vector.hpp"
#include "Statistics.hpp"

//class Vector;
class Worm;
class MDAEvent;

#define MAX_POP 11550

 

//class to represent a population
//A container class for the Host class and also provides a size distribution function


class Population {
    
    friend std::ostream& operator<<(std::ostream& ostr, const Population&);
    
public:
    
    Population(const std::string paramsfile);
    
    ~Population(){
        if (popSize != NULL)
            delete[] popSize;
        if(populationDistribution != NULL)
            delete populationDistribution;
        
        
    }
    
    void loadPopulationSize(const std::string filename);
    int getMaxAge() const;
    void initHosts(std::string distType);
    double getPopSize() ;
    double getPrevalence()  const;
    double getLarvalUptakebyVector(double r1, double kappas1, Vector::vectorSpecies species) const;
    double getBedNetCoverage()  const;
    
    void updateBedNetCoverage(double cov = -1, double sysComp = -1);
    void updateImportationRate(double factor);
    
    void evolve(double dt, const Vector& vectors, const Worm& worms);
    void ApplyTreatment(MDAEvent& mda,  Worm& worms);
    void saveCurrentState(int year, std::string sname);
    void resetToYear(int year);
    void clearSavedYears();
    
    std::vector<std::string> printRandomVariableNames() const;
    std::vector<double> printRandomVariableValues() const;

    
private:
    
    double calcU0(double coverage, double sigma);
    void setUB(Host&  h);
    void setU(Host& h, double sigmaMDA, double sigmaBednets);
    void rmvnorm(const int n, const gsl_vector *mean, const gsl_matrix *var, gsl_vector *result);
    
    //param sampling
    std::default_random_engine randgen;
    std::discrete_distribution<int>* populationDistribution;
    int* popSize;
    
    //The hosts
    Host host_pop[MAX_POP];
    
    int size;
    
    const int maxAge = 4;
    double tau; // death rate of host    

   
    double sysCompMDA;
    double sysCompBednets;
    double defSysCompMDA;
    double defSysCompBednets;
    
    
    double sigma2Bednets;    //amount of systematic bednet adherance, if 0, no systematic adherence so completely random which individuals comply in each round, if 1 then same individuals comply in each round
    double sigma2MDA;
    
    double sigmaBednets;
    double sigmaMDA;
    
                        //this is the square root of variance of normal distribution from which probability of usibg bednet is drawn
    double u0CompBednets;     //Need to set Host::uCompN for bednet compliance. This is the mean of a normal distribution from which a random value is take to determine whether host complies or not
                        //Set to a value that will give a probability of compliance equal to the specified coverage covN
    double u0CompMDA;
   
    
    double k;     //shape parameter for gamma distrib used to get biteRisk
    
    double rhoBU;   //correlation between non-compliance and bite risk
    double rhoCN;   //correlation of non-compliance between bed-nets and MDA (Chemotherapy)
    
    double TotalBiteRisk; //sum for whole population
    
    
    //max values for random parameters
    double mink;
    double maxk;
    double maxaImp;
    double meanaImp;
    
    double aImp;
    double aImp_original;
    double bedNetCov;
    double mdaCoverage;
   
    
    //saved years
    
    
    typedef struct {
        
        std::vector<hostState> data;
        int year;
        double aImp;
        double sysCompMDA;
        double sysCompBednets;
        std::string scenario; //for debugging only. The scenario that saved this year
        
    } savedYear;
    
    std::vector<savedYear> savedYears;

    
};


#endif /* Population_hpp */
