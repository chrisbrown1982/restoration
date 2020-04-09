//
//  Model.cpp
//  transfil
//
//  Created by Paul Brown on 27/01/2017.
//  Copyright © 2017 Paul Brown. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>

#include "Model.hpp"
#include "ScenariosList.hpp"
#include "Scenario.hpp"
#include "Population.hpp"
#include "Vector.hpp"
#include "Worm.hpp"
#include "../../tracing.hpp"
#include "loops.hpp"

extern bool _DEBUG;
extern Statistics stats;

//tmp fix
void getRandomParameters(int index, std::vector<double>& k_vals, std::vector<double>& v_to_h_vals, std::vector<double>& aImp_vals, int replicates);


void Model::runScenarios(ScenariosList& scenarios, Population& popln, Vector& vectors, Worm& worms, int replicates, double timestep, int index){
    
    std::cout << std::endl << "Index " << index << " running " << scenarios.getName() << " with " << scenarios.getNumScenarios() << " scenarios" << std::endl;
    
    std::cout << std::unitbuf;
    std::cout << "Progress:  0%";
    
    Output currentOutput(scenarios.getNumYears(), scenarios.getBaseYear());
    currentOutput.saveRandomNames(popln.printRandomVariableNames(), vectors.printRandomVariableNames()); //names of random vars to be printed
    
    dt = timestep;
    
    scenarios.openFilesandPrintHeadings(index, currentOutput);
    
    
    //Temporary fix. Read file containing random values for v_to_h, aImp and k
    
    std::vector<double> k_vals;
    std::vector<double> v_to_h_vals;
    std::vector<double> aImp_vals;
    
    //getRandomParameters(index, k_vals, v_to_h_vals, aImp_vals, replicates);
    

    
    __begin_loop(LOOP54);
    for (int rep = 0; rep < replicates; rep++){
        __begin_iteration(LOOP54);
        
        currentYear = 0;
        popln.clearSavedYears();
        vectors.clearSavedYears();
        currentOutput.initialise(); //sets outputs to zero
        scenarios.resetMDACount(); //reset number of treatments still to come
        
        //functions temporarily altered to allow random value to be passed in
        
        std::string distType = stats.selectDistribType();
        //create a new host population with random size, ages and compliance p vals and bite risk. Generates new value for k and aImp
        popln.initHosts(distType);
        //generate vector to host ratio value and reset L3 to initial value
        vectors.reset(distType);
        
        
        //save these values for printing later
        currentOutput.saveRandomValues(popln.printRandomVariableValues(), vectors.printRandomVariableValues());
        
        //baseline prevalence
        burnIn(popln, vectors, worms, currentOutput);                    //should be at least 100 years
        
        //Run each scenario
        __begin_loop(LOOP78);
        for (int s = 0; s < scenarios.getNumScenarios(); s++){
            __begin_iteration(LOOP78);

            Scenario& sc = scenarios[s];
            
            if(_DEBUG) std::cout << std::endl << sc.getName() << " starts year " << sc.getStartYear() << std::endl;
            
            if(sc.getStartYear() != currentYear){
                
                //reset to the end of this year
                currentYear = sc.getStartYear();
                popln.resetToYear(currentYear);    //worms and aImp
                vectors.resetToYear(currentYear);  //L3
                currentOutput.resetToYear(currentYear); //MDA count 
            }
            
            //evolve, saving any specified years along the way
            __begin_loop(LOOP94);
            for (int y = 0; y < sc.getNumYearsToSave(); y++) {
                __begin_iteration(LOOP94);
                evolveAndSave(y, popln, vectors, worms, sc, currentOutput);
                __end_iteration(LOOP94);
            }
            __end_loop(LOOP94);
            
            //done for this scenario, save the prevalence values for this replicate
            if(!_DEBUG) sc.printResults(rep, currentOutput);
            __end_iteration(LOOP78);
        }//end of each scenario
        __end_loop(LOOP78);
        if(!_DEBUG){
            std::cout << "\b\b\b\b";
            std::cout << std::setw(3) << int(rep*100/replicates) << "%";
        }
        __end_iteration(LOOP54);
    } // end for each rep
    __end_loop(LOOP54);
    
    scenarios.closeFiles();
    //finished
    
}


void Model::burnIn(Population& popln, Vector& vectors, const Worm& worms, Output& currentOutput){
    
    //burn in period. Don't need to worry about drugs
    //just save final state
    
    int steps = 12 * std::max(4, popln.getMaxAge())/dt; //one step is 1 * dt months, run for 100 years, Must be at least maxAge
    
    __begin_loop(LOOP120);
    for(int i = 0; i < steps; i++){
        __begin_iteration(LOOP120);
        
        //updates number of worms in each hosts and increments host age
        popln.evolve(dt, vectors, worms);
        
        //update larval density in the vector population according to new mf levels in host polution
        vectors.updateL3Density(popln, worms);
        
        //save prevalence at end of January of last year for consistency
        if (i == (steps-12))
            currentOutput.saveYear(0, popln.getPrevalence());
        
        
        __end_iteration(LOOP120);
    }
    __end_loop(LOOP120);
    //Does not update currentTime as we aren't simulating yet
    //This is now the end of 2005


}


void Model::evolveAndSave(int y, Population& popln, Vector& vectors, Worm& worms, Scenario& sc, Output& currentOutput){
    
    //advance tot_t years, one month at a time
    //advances to the end of targetYear, saving prevalence at the end of the first month of each year along the way
    
    int months;
    bool newyear;
    
    int targetYear = sc.getYearToSave(y);
    
    if((months = 12 * (targetYear - currentYear)) <= 0){
        std::cout << "Error in Model::evolveAndSave. Target year must be beyond current year!" << std::endl;
        exit(1);
    }

    
    __begin_loop(LOOP157);
    for (int t = 0; t < months; t += dt){
        __begin_iteration(LOOP157);

        if((newyear = ((t%12) == 0)))//update bed nets and importation rate
            sc.updateForNewYear(popln, ++currentYear);
        
        //updates number of worms in each hosts and increments host age
        popln.evolve(dt, vectors, worms);

        //update larval density in the vector population according to new mf levels in host polution
        vectors.updateL3Density(popln, worms);
        
        //output prevalence at end of Jan of each year
        if(newyear)
            currentOutput.saveYear(currentYear, popln.getPrevalence());
  
        if (sc.treatmentDue(currentYear, t)){
            //treatment always at end of Jan or June
            MDAEvent mda = sc.getMDAEvent();
            popln.ApplyTreatment(mda, worms); //treated set. alters M, WM, WF, using covMDA set above
            //should record to output that MDA has been done this year
            currentOutput.saveMDAEvent(currentYear, mda);
            if(_DEBUG) std::cout << mda.getType() << " year " << currentYear << " at " << mda.getCoverage() << std::endl;
        }
        //next t
        __end_iteration(LOOP157);
    }
    __end_loop(LOOP157);
    //done
    
    if (y < (sc.getNumYearsToSave()-1)){ //not finished this scenario
        popln.saveCurrentState(targetYear, sc.getName()); //worms and importation rate. Scenario name just needed for debugging
        vectors.saveCurrentState(targetYear); //larval density
        
        if(_DEBUG) std::cout << sc.getName() << " saving year " << targetYear << std::endl;

    }
    
}

void getRandomParameters(int index, std::vector<double>& k_vals, std::vector<double>& v_to_h_vals, std::vector<double>& aImp_vals, int replicates){
    
    
    std::stringstream str;
    str << "RandomParamIndex" << index <<".txt";
    std::string fname = str.str();
    
    
    std::ifstream infile(fname, std::ios_base::in);
    
    if(!infile.is_open()){
        std::cout << "Error in Model::createScenarios. Cannot read file " << fname << std::endl;
        exit(1);
    }

    std::string line;
    
    while (getline(infile, line)){
       
         std::istringstream linestream(line);
        double k = -1.0, v_to_h = -1.0, aImp = -1.0;
        
        linestream >> v_to_h >> k >> aImp;
        
        if (k < 0 || v_to_h < 0 || aImp < 0){
            std::cout << "Error in Model::createScenarios. Error reading file " << fname << std::endl;
            exit(1);
            
        }
       
        k_vals.push_back(k);
        aImp_vals.push_back(aImp);
        v_to_h_vals.push_back(v_to_h);
       
    }

    
    infile.close();
    
    if((k_vals.size() < replicates) || (v_to_h_vals.size() < replicates) || (aImp_vals.size() < replicates)){
        std::cout << "Error in Model::runScenarios. " << fname << " is too short for " << replicates << "replicates" << std::endl;
        exit(1);
        
    }
    
    
}






