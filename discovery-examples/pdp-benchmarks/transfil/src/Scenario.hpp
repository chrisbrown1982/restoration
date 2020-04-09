//
//  Scenario.hpp
//  transfil
//
//  Created by Paul Brown on 02/03/2017.
//  Copyright © 2017 Paul Brown. All rights reserved.
//

#ifndef Scenario_hpp
#define Scenario_hpp

#include <vector>
#include <string>
#include <fstream>
#include "MDAEvent.hpp"

class Output;
class Population;


//class to represent a single scenario

class Scenario {
    
public:

    
    Scenario(int numYears, int syr) : startYear(syr){
        
        initialise(numYears);
        name = "Scenario";
        
    };
    
    Scenario(int numYears, int syr, std::string nm) : startYear(syr), name(nm) {
        
        initialise(numYears);
        
    };
    
    Scenario(int numYears, int syr, MDAEvent mda) : startYear(syr), MDAprot(mda) {
        
        initialise(numYears);
        name = "Scenario";
        
    };
    
    Scenario(int numYears, int syr, std::string nm, MDAEvent mda) : startYear(syr),  MDAprot(mda), name(nm) {
        
        initialise(numYears);
        
    };

    
    std::string getName() const {
        return name;
    }
    
    MDAEvent& getMDAEvent() {
        
        return MDAprot;
    }
    
    void updateForNewYear(Population& popln, int year);
    bool treatmentDue(int year, int month);
    
    void setImportationFactor(std::vector<double>& fac);
    void setBedNetCoverage(std::vector<double>& cov);
    void setYearsToSave(std::vector<int>& yrs);
    
    double getBedNetCoverage(int year) const;
    double getImportationFactor(int year) const;
    int getStartYear() const;
    int getNumYearsToSave() const;
    int getYearToSave(int idx) const;
    
    void openFileandPrintHeadings(std::string region, Output& results);
    void printResults(int repnum, Output& results);
    void closeFile();
    
    void resetMDACount();
    
    std::vector<double>bedNetCoverage;
    
protected:
    
    int startYear;
    MDAEvent MDAprot;
    
    void initialise(double numYears);
    
    
    std::vector<int> yearsToSave;
    std::vector<double> aImpFactor;
    std::string name;
    std::ofstream myfile;
    
    int MDADone;
    
    
    
};

#endif /* Scenario_hpp */
