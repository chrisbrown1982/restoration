//
//  ScenariosList.cpp
//  transfil
//
//  Created by Paul Brown on 28/02/2017.
//  Copyright © 2017 Paul Brown. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <limits.h>
#include "ScenariosList.hpp"

extern bool _DEBUG;

void ScenariosList::loadaImpFactor(const std::string aImpfile){
    
    
    //should be one line for each scenario with one value for each year
    
    std::ifstream infile(aImpfile, std::ios_base::in);
    
    if(!infile.is_open()){
        std::cout << "Error in Model::loadaImpFactor. Cannot read file " << aImpfile << std::endl;
        exit(1);
    }
    
    std::string line;
    std::istringstream iss;
    std::vector<double> valsForScenario;
    int scenario = 0;
    
    while (getline(infile, line)) {
        
        if(scenario == scenarios.size()){
            std::cout << "Warning in Model::loadaImpFactor. File " << aImpfile << " is too long. Ignoring extra data after line " << scenario << std::endl;
            break;
        }
        
        std::string factor;
        iss.clear();
        iss.str(line);
        valsForScenario.clear();
        
        while(iss){
            //for each year in this scenario
            iss >> factor;  //keeps repeating last entry at end of line
            if(factor.length()){
                
                if (valsForScenario.size() == numYears){
                  //  std::cout << "Warning in Model::loadaImpFactor. File " << aImpfile << " scenario  " << (scenario+1) << " has too many entries. Ignoring excess years." << std::endl;
                    break;
                }else
                    valsForScenario.push_back(atof(factor.c_str()));
                
            }else
                break;
            
        }
        //end of line. copy years to this scenario, padding at start with ones if not enough years
        
        int n = numYears-(int)valsForScenario.size();
        if (n > 0)
            valsForScenario.insert(valsForScenario.begin(), n, 1.0);
    
        scenarios[scenario++].setImportationFactor(valsForScenario);
    
        
    }
    
    infile.close();
    
    if(scenario < scenarios.size())
        std::cout << "Warning in Model::loadaImpFactor. File " << aImpfile << " is too short. aImp will not vary after scenario " << scenario << std::endl;

    
}


void ScenariosList::loadBedNetData(const std::string bedNetfile){
    
    
    std::ifstream infile(bedNetfile, std::ios_base::in);
    std::vector<double> bedNets;
    
    if(!infile.is_open()){
        std::cout << "Error in Model::loadBedNetData. Cannot read file " << bedNetfile << std::endl;
        exit(1);
    }
    
    double val;
    
    while(!infile.eof()){
        if(bedNets.size() == numYears){
            std::cout << "Warning in Model::loadBedNetData. File " << bedNetfile << " has too many entries. Using only the first " << numYears << std::endl;
            break;
        }
        infile >> val;
        bedNets.push_back(val);
    }
    infile.close();
    
    if(bedNets.size() < numYears){
        std::cout << "Warning in Model::loadBedNetData. File " << bedNetfile << " not enough entries. Extrapolating from year " << bedNets.size()-1 << " to end." << std::endl;
        int year = int(bedNets.size());
        for (int i = year; i < numYears; i++)
            bedNets.push_back(bedNets[year-1]);
    }
    
    //at present this doesn't vary between scenarios, but potentially it could
    
    for(int s = 0; s < scenarios.size(); s++)
        scenarios[s].setBedNetCoverage(bedNets);
    
    
}



const Scenario& ScenariosList::getScenario(int snum) const {
    
    //get required interventions for this scenario
    
    
    return scenarios[snum]; 
    
}

unsigned long ScenariosList::getNumScenarios() const {
    
    return scenarios.size();
    
}

//Region specific scenarios

void ScenariosList::createScenarios(const std::string scenariosFile, const std::string aImpFile, const std::string bedNetFile, const std::string opDir){
    
    //crearte a list of rthe different scenarios
    
    std::ifstream infile(scenariosFile, std::ios_base::in);
    
    if(!infile.is_open()){
        std::cout << "Error in Model::createScenarios. Cannot read file " << scenariosFile << std::endl;
        exit(1);
    }
    
    int finalYear = baseYear = 0;
    
    infile >> name >> baseYear >> finalYear;
    
    if(finalYear <= 0 || finalYear <= baseYear){
        std::cout << "Error in Model::createScenarios. In file " << scenariosFile << " the start and end years are not properly defined." <<std::endl;
        exit(1);
    }
    
    numYears = (finalYear - baseYear + 1);
    
    std::string line;
    
    while (getline(infile, line)){
        
        //each line in this file consists of
        //name  startYear [mdaStartYear mdaCov mdaFreq numMDA]
        
        if (line.length() == 0) continue; //ignore blank lines
        std::istringstream linestream(line);
        
        std::string sname;
        int syear = -1;
        int mdastart = -1, mdafreq = 0, mdanum = -1;
        double mdacov = 0.0, sysComp = -1;
        std::string mdaType;
        
        linestream >> sname >> syear;
        
        if(scenarios.empty() && syear > 0){
            std::cout << "Error in Model::createScenarios. In file " << scenariosFile << " the first scenario must begin at year zero." <<std::endl;
            exit(1);
        }else if (syear < 0){
            std::cout << "Error in Model::createScenarios. The file " << scenariosFile << " is badly formatted." <<std::endl;
            exit(1);
        }else if (syear >= numYears){
            std::cout << "Error in Model::createScenarios. File " << scenariosFile << " at line " << scenarios.size()+1 << ". Scenario start year is beyond the end of the simulation." << std::endl;
            exit(1);
        }
        //mda optional
        
        
        linestream >> mdastart >> mdacov >> mdafreq >> mdanum >> mdaType >> sysComp; //sysComp will have default value if not in file
            
        if (mdanum < 0 && mdastart >= 0){ //line incomplete
            std::cout << "Error in Model::createScenarios. Badly formatted file " << scenariosFile << " at line " << scenarios.size()+1 << std::endl;
            exit(1);
        }else if (mdastart < 0 || mdafreq <= 0 || mdacov <= 0.0 || mdanum <= 0){ //ignore invalid values
            scenarios.push_back(Scenario(numYears, syear, sname));
            if(_DEBUG)  std::cout << sname << " starting year " << syear << " has no treatment"  << std::endl;
        }else if ( 12 % mdafreq ){
            std::cout << sname << " MDA frequency must be a factor of 12 "  << std::endl;
            exit(1);

        }else{
            scenarios.push_back(Scenario(numYears, syear, sname, MDAEvent(mdastart, mdafreq, mdanum, mdacov, mdaType, sysComp)));
            if(_DEBUG) std::cout << sname << " starting year " << syear << " has " << mdanum << " rounds of " << mdaType << " treatment every " << mdafreq << " months with " << mdacov << " coverage, starting year " << mdastart << std::endl;

        }
        

        
    }
    infile.close();
    
    if(!scenarios.size()){
        std::cout << "Error in Model::createScenarios. No scenarios found in file " << scenariosFile << "." <<std::endl;
        exit(1);
    }

    //Now calculate the years that we need to save as each scenario runs. This is done as follows
    //1) For scenario n, examine the start year for scenarios n+1 ... end
    //2) if their start year is both later than the start year for scenario n AND earlier than any other year that scenario n will save THEN
    // save this year when running scenario n.
    //3) If this year has a start year earier than scenario n, then stop
    
    for (int s = 0; s < scenarios.size(); s++){
        
        std::vector<int> toSave;
        int currentStartYear = scenarios[s].getStartYear();
        int earliestYearSaved = INT_MAX;
        
        for (int t = s+1; t < scenarios.size(); t++){
            
            int newStartYear = scenarios[t].getStartYear();
            
            if (newStartYear <= currentStartYear)
                break;
            else if (newStartYear < earliestYearSaved){

                toSave.push_back(newStartYear);
                earliestYearSaved = (earliestYearSaved < newStartYear)?earliestYearSaved:newStartYear;
            }
            
        }
        toSave.push_back(numYears-1);
        std::sort(toSave.begin(), toSave.end());
        scenarios[s].setYearsToSave(toSave);

        if(_DEBUG){
            std::cout << scenarios[s].getName() << " will save years :";
            for (int i =0; i < toSave.size(); i++)
                std::cout << " " << toSave[i];
            std::cout << std::endl;
            
        }
        
    }
    
    outputDir = opDir;

    //set the importation factors, a vector of yearly values
    if(aImpFile.length())
        loadaImpFactor(aImpFile);
    
    
    //same for all scenarios
    if(bedNetFile.length())
        loadBedNetData(bedNetFile);
    
    
}


void ScenariosList::openFilesandPrintHeadings(int index, Output& results) {
    
    for(int s = 0; s < scenarios.size(); s++){
        std::stringstream fname;
        fname << outputDir << index << "_" << name;
         scenarios[s].openFileandPrintHeadings(fname.str(), results) ;
    }
    
}


void ScenariosList::closeFiles(){
    
    for(int s = 0; s < scenarios.size(); s++)
        scenarios[s].closeFile();
}

void ScenariosList::resetMDACount(){
    
     for(int s = 0; s < scenarios.size(); s++)
         scenarios[s].resetMDACount();
}







