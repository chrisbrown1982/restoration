//
//  Output.hpp
//  transfil
//
//  Created by Paul Brown on 24/03/2017.
//  Copyright © 2017 Paul Brown. All rights reserved.
//

#ifndef Output_hpp
#define Output_hpp

#include <vector>
#include <string>

class MDAEvent;

//class to define the outputs of the model

class Output {
    
public:
    
    Output& operator=(const Output&);
    
    Output(){};
    Output(int years);
    Output(int years, int baseYear);
    
    void initialise();
    void resetToYear(int year);
    void saveYear(int year, double prev);
    void saveMDAEvent(int year, MDAEvent& mda);
    int length() {return numYears;}
    int getBaseYear() {return baseYear;}
    
    void saveRandomNames(std::vector<std::string> names);
    void saveRandomNames(std::vector<std::string> names1, std::vector<std::string> names2);
    void saveRandomValues(std::vector<double> vals);
    void saveRandomValues(std::vector<double> vals1, std::vector<double> vals2);
    int getNumRandomVars();
    std::string&  getRandomVarNames(int idx);
    double  getRandomVarValues(int idx);
    
    
    std::vector<double> prevalence;
    std::vector<int> numMDAEvents;
    std::vector<double> MDACoverage;
    std::vector<std::string> MDAType;
    std::vector<double> MDAsysComp;
    
    
    
    
private:
    void reserve();
    int numYears;
    int baseYear;
    
    std::vector<std::string> randomVarNames;
    std::vector<double> randomVarValues;

    
};

#endif /* Output_hpp */
