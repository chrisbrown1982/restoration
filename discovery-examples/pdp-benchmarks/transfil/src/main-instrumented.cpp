// pointer to classes example

#include <string>
#include <sstream>
#include <iostream>
#include <sys/time.h>
#include <string.h>

#include "Model.hpp"
#include "ScenariosList.hpp"
#include "Vector.hpp"
#include "Worm.hpp"
#include "Statistics.hpp"
#include "Population.hpp"
#include "../../tracing.hpp"

/*
 to compile on mac

 clang -I. -I/usr/local/include  -L/usr/local/lib  -Wall -O3 -std=c++11 -stdlib=libc++  -lm -lgsl -lgslcblas main.cpp ScenariosList.cpp Scenario.cpp Population.cpp Host.cpp Model.cpp Output.cpp Vector.cpp Worm.cpp Statistics.cpp MDAEvent.cpp -o transfil
 
 on nero
 
 scl devtoolset-3 enable
 
 g++ -I. -I/usr/include  -L/usr/lib64  -Wall -O3 -std=c++11  -lm -lgsl -lgslcblas main.cpp ScenariosList.cpp Scenario.cpp Population.cpp Host.cpp Model.cpp Output.cpp Vector.cpp Worm.cpp Statistics.cpp -o transfil

 */

bool _DEBUG = false;
Statistics stats;


int main(int argc, char **argv) {
    __begin_tracing();
    
    if (argc < 2){

        std::cout << "transfil index -s <scenarios_file>  -n <pop_file> -p <parameters_file=\"parameters.txt\"> -a <importation_file=\"\"> -b <bednet_file=\"\"> -r <replicates=1000> -t <timestep=1> -o <output_directory>" << std::endl;
        return 1;
    }
    
    struct timeval tv1, tv2;
    gettimeofday(&tv1, NULL);
    
    int replicates = 0;
    double dt = 1.0;
    std::string popFile;
    std::string impFile("");
    std::string bedNetFile("");
    std::string paramsfile("");
    std::string scenariosFile("");
    std::string opDir("");

    int index = 0;
    if (!strcmp(argv[1], "DEBUG")){
        _DEBUG=true;
        replicates=1;
    }else
        index = atoi(argv[1]); //used for labelling output files
    
    for (int i = 2; i < (argc-1); i+=2){
        
        if (!strcmp(argv[i],  "-r")){
            if(!_DEBUG) replicates = atoi(argv[i+1]);
        }else if (!strcmp(argv[i],  "-s"))
            scenariosFile = argv[i+1];
        else if (!strcmp(argv[i],  "-n"))
            popFile = argv[i+1];
        else if (!strcmp(argv[i],  "-a"))
            impFile = argv[i+1];
        else if (!strcmp(argv[i],  "-b"))
            bedNetFile = argv[i+1];
        else if (!strcmp(argv[i],  "-p"))
            paramsfile = argv[i+1];
        else if (!strcmp(argv[i],  "-t"))
            dt = atof(argv[i+1]);
        else if (!strcmp(argv[i],  "-o"))
            opDir = argv[i+1];
        else{
            std::cout << "Error: unknown command line switch " << argv[i] << std::endl;
            return 1;
        }
        
    }
    
    for (int i = 0; i < argc; i++)
        std::cout << argv[i] << " ";
    std::cout << std::endl << std::endl;
    
    //validate
    
    if(scenariosFile.length() == 0){
        std::cout << "Error: Scenarios file undefined."<<std::endl;
        return 1;
    }
    if(popFile.length() == 0){
        std::cout << "Error: Population size file undefined."<<std::endl;
        return 1;
    }
    if(!replicates){
        replicates = 1000;
        std::cout << "Replicates undefined so using default value of " << replicates << std::endl;
    }
    if(paramsfile.length() == 0){
        paramsfile = "../input/parameters.txt";
        std::cout << "Parameters file undefined so using default value of " << paramsfile <<std::endl;
    }
    if(impFile.length() == 0)
         std::cout << "Importation rate factor file undefined so assuming that the rate is constant." << std::endl;
    if(bedNetFile.length() == 0)
        std::cout << "Bed net coverage file undefined so assuming there is no coverage." << std::endl;
    std::cout << std::endl;
    
    if(opDir.length() == 0)
        opDir = "./";
    else if(opDir.back() != '/')
        opDir = opDir + "/";

    
    paramsfile = opDir+paramsfile;
    popFile = opDir+popFile;
    scenariosFile = opDir+scenariosFile;
    
    if(bedNetFile.length())bedNetFile = opDir+bedNetFile;
    if(impFile.length())impFile = opDir+impFile;
    

    Model model;
    
    //Create the Vector, Worm and Host population objects
    Vector vectors(paramsfile);
    Worm worms(paramsfile);
    Population hostPopulation(paramsfile);
    hostPopulation.loadPopulationSize(popFile);

    //Create Scenarios
    ScenariosList Scenarios;
    Scenarios.createScenarios(scenariosFile, impFile, bedNetFile, opDir);
    
    //Run
    model.runScenarios(Scenarios, hostPopulation, vectors, worms, replicates, dt, index);

    
    gettimeofday(&tv2, NULL);
    double timesofar = (double)(tv2.tv_usec-tv1.tv_usec)/1000000.0 + (double)(tv2.tv_sec-tv1.tv_sec);
    std::cout << std::endl << "Completed successfully in " << timesofar << " secs." << std::endl;

    return 0;
}

























