//
//  Model.hpp
//  transfil
//
//  Created by Paul Brown on 27/01/2017.
//  Copyright © 2017 Paul Brown. All rights reserved.
//

#ifndef Model_hpp
#define Model_hpp


#include <vector>
#include <fstream>
#include <random>

#include "Output.hpp"


class Scenario;
class ScenariosList;
class Population;
class Vector;
class Worm;


class Model {
    
    
public:

    void runScenarios( ScenariosList& scenarios, Population& popln, Vector& vectors, Worm& worms, int replicates, double timestep, int index);
   
protected:
    
    void burnIn(Population& popln, Vector& vectors, const Worm& worms, Output& currentOutput);
    void evolveAndSave(int y, Population& popln, Vector& vectors, Worm& worms, Scenario& sc, Output& currentOutput);
    
    int currentYear;
    double dt;
    
};


#endif /* Model_hpp */
