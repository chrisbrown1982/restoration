//
//  Output.cpp
//  transfil
//
//  Created by Paul Brown on 24/03/2017.
//  Copyright © 2017 Paul Brown. All rights reserved.
//

#include "Output.hpp"
#include "MDAEvent.hpp"

Output::Output(int years): numYears(years){
    
    reserve();
   
}

Output::Output(int years, int byear): numYears(years), baseYear(byear){
    
    reserve();
    
}


void Output::reserve(){
    
    prevalence.reserve(numYears);
    numMDAEvents.reserve(numYears);
    MDACoverage.reserve(numYears);
    MDAType.reserve(numYears);
    MDAsysComp.reserve(numYears);
}


void Output::initialise(){
    
    //called at the start of a new replicate
    
    prevalence.assign(numYears,0.0);
    numMDAEvents.assign(numYears,0);
    MDACoverage.assign(numYears,-1.0);
    MDAType.assign(numYears,std::string(""));
    MDAsysComp.assign(numYears, -1.0);
    
}

void Output::resetToYear(int year){
    
    //called at start of a new scenario. Remove any data that will be overwritten
    
    std::fill(prevalence.begin()+year+1, prevalence.end(), 0.0);
    std::fill(numMDAEvents.begin()+year+1, numMDAEvents.end(), 0);
    std::fill(MDACoverage.begin()+year+1, MDACoverage.end(), -1.0);
    std::fill(MDAType.begin()+year+1, MDAType.end(), std::string(""));
    std::fill(MDAsysComp.begin()+year+1, MDAsysComp.end(), -1.0);

    
}

Output& Output::operator=(const Output& op){
    
    //copies data to a scenario when done
    
    prevalence = op.prevalence;
    numMDAEvents = op.numMDAEvents;
    MDACoverage = op.MDACoverage;
    MDAType = op.MDAType;
    MDAsysComp = op.MDAsysComp;
    return *this;
}

void Output::saveYear(int year, double prev){
    
    prevalence[year] = prev;
    
}

void Output::saveMDAEvent(int year, MDAEvent& mda){
    
    numMDAEvents[year]++;
    MDACoverage[year] = mda.getCoverage();
    MDAType[year] = mda.getType();
    MDAsysComp[year] = mda.getCompliance();
    
}


void Output::saveRandomNames(std::vector<std::string> names){
    
    randomVarNames = names;
    
    
}

void Output::saveRandomNames(std::vector<std::string> names1, std::vector<std::string> names2){
    
    randomVarNames = names1;
    for (int i = 0; i < names2.size(); i++)
        randomVarNames.push_back(names2[i]);
    
}


void Output::saveRandomValues(std::vector<double> vals){
    
    //Values corresponding to names passed to above
    randomVarValues = vals;
    
}

void Output::saveRandomValues(std::vector<double> vals1, std::vector<double> vals2){
    
    //Values corresponding to names passed to above
     randomVarValues = vals1;
    for (int i = 0; i < vals2.size(); i++)
        randomVarValues.push_back(vals2[i]);
    
}

int Output::getNumRandomVars(){
    
    //used for indenting results spreadsheet by right number of cols
    
    return int(randomVarNames.size());
    
}

std::string&  Output::getRandomVarNames(int idx){
    
    return randomVarNames[idx];
    
}

double  Output::getRandomVarValues(int idx){
    
    return randomVarValues[idx];
    
}






