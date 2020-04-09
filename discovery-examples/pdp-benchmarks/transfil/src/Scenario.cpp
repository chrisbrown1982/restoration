//
//  Scenario.cpp
//  transfil
//
//  Created by Paul Brown on 02/03/2017.
//  Copyright © 2017 Paul Brown. All rights reserved.
//

#include "Scenario.hpp"
#include "Output.hpp"
#include "Population.hpp"
#include <iostream>
#include <sstream>

void Scenario::updateForNewYear(Population& popln, int year){
    
  //Update the importation rate and bed net coverage eeach year
    
    // This has effect on Vector::L3 below and also on host::react
    popln.updateBedNetCoverage(getBedNetCoverage(year)); //This now has an optional 2nd input, sysComp
    //imporation rate affects host::react() below
    popln.updateImportationRate(getImportationFactor(year));

    
}


bool Scenario::treatmentDue(int year, int month){
    
    return MDAprot.treatmentDue(year, month);
    
}



void Scenario::setImportationFactor(std::vector<double>& fac){
    
    aImpFactor = fac;
    
}


void Scenario::setBedNetCoverage(std::vector<double>& cov){
    
    bedNetCoverage = cov;
}

void Scenario::setYearsToSave(std::vector<int>& yrs){
    
    yearsToSave = yrs;
    
}


void Scenario::initialise(double numYears){
    
    //called by constructor
    
    aImpFactor.assign(numYears ,1.0);       //default is no variation in aImp
    bedNetCoverage.assign(numYears, 0.0);   //default is no bednet
  //  MDADone = 0;
    
}


double Scenario::getBedNetCoverage(int year) const{
    
    return (year < bedNetCoverage.size())? bedNetCoverage[year]: 0.0;
    
}


double Scenario::getImportationFactor(int year) const{
    
    return (year < aImpFactor.size())? aImpFactor[year]: 1.0;
    
}

int Scenario::getStartYear() const{
    
    return startYear;
}

int Scenario::getNumYearsToSave() const {
    
    return int(yearsToSave.size());
}

int Scenario::getYearToSave(int idx) const {
    
    return yearsToSave[idx];
}

void  Scenario::openFileandPrintHeadings(std::string region, Output& results){
    
    std::string fname = region + "_" + name + ".txt";
    
    myfile.open(fname, std::ios_base::out);
    
    if (!myfile.is_open()){
        
        std::cout << "Error writing results to file " << fname << std::endl;
        exit(1);
        
    }
    
    std::string indent(results.getNumRandomVars()+1, '\t');
    
    myfile << name << std::endl;
    myfile << indent << "Year";
    for (int i = 0; i < results.length(); i++)
        myfile << "\t" << (i+results.getBaseYear());
    myfile << std::endl;
    
    myfile << indent <<  "Bed net coverage";
    for (int i = 0; i < results.length(); i++)
        myfile << "\t" << getBedNetCoverage(i);
    myfile << std::endl;
    
    myfile << indent << "Importation rate factor";
    for (int i = 0; i < results.length(); i++)
        myfile << "\t" << getImportationFactor(i);
    myfile << std::endl;
    
    
    
}


void Scenario::printResults(int repnum, Output& results){
    
    
    if(!repnum){
        
        //print MDA events
        
        std::string indent(results.getNumRandomVars()+1, '\t');
        
        myfile << indent << "MDA number";
        for (int i = 0; i < results.length(); i++)
            myfile << "\t" << results.numMDAEvents[i];
        myfile << std::endl;
        
        myfile << indent << "MDA coverage";
        for (int i = 0; i < results.length(); i++)
            if(results.MDACoverage[i] >= 0)
                myfile << "\t" << results.MDACoverage[i];
            else
                myfile << "\t";
        myfile << std::endl;
        
        myfile << indent << "MDA type";
        for (int i = 0; i < results.length(); i++)
            myfile << "\t" << results.MDAType[i];
        myfile << std::endl;
        
        myfile << indent << "Systematic compliance";
        for (int i = 0; i < results.length(); i++)
            if(results.MDAsysComp[i] >= 0)
                myfile << "\t" << results.MDAsysComp[i];
            else
                myfile << "\t";
        myfile << std::endl;
        
        myfile << "ID";
        
        for (int i =0; i < results.getNumRandomVars(); i++)
            myfile << "\t" << results.getRandomVarNames(i);
        myfile << std::endl;

        
    }
    
    myfile << (repnum+1);
    for (int i =0; i < results.getNumRandomVars(); i++)
        myfile << "\t" << results.getRandomVarValues(i);
    myfile << "\t";
    
    for (int i = 0; i < results.length(); i++)
        myfile << "\t" << results.prevalence[i];
    myfile << std::endl;
    
}

void Scenario::closeFile(){
    
    myfile.close();
}

void Scenario::resetMDACount(){
    
   // MDADone = 0;
    MDAprot.reset();
}






