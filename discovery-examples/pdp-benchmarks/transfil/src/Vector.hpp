//
//  Vector.hpp
//  transfil
//
//  Created by Paul Brown on 08/03/2017.
//  Copyright © 2017 Paul Brown. All rights reserved.
//

#ifndef Vector_hpp
#define Vector_hpp

#include <string>
#include <vector>

class Vector;
class Population;
class Worm;

//class to represent the moquito population.
//Only one instance of this as we don't model then seperately!


class Vector {
    
    friend std::ostream& operator<<(std::ostream& ostr, const Vector& vec);
    
public:
    
    Vector(const std::string paramsfile);
    void reset(std::string distType);
    void updateL3Density(const Population& popln, const Worm& worms);
    double averageNumBites() const;
    double probBitesThroughNet() const;
    double getL3Density() const;
    void saveCurrentState(int year);
    void resetToYear(int year);
    void clearSavedYears();

    std::vector<std::string> printRandomVariableNames() const;
    std::vector<double> printRandomVariableValues() const;
    
    enum vectorSpecies {Anopheles, Culex};
    
    
private:
    
    double lambda;          //bites per month
    double v_to_h;          //vector to host ratio
    const double sN = 0.03; //prob successful feeding though bednet
    const double dN=0.41;   //prob that mosquito dies on bednet
    
    double L3;              //larval density in vector population
    double initL3;
    double kappas1, r1;     //larval uptake from bite
    double g;               //Proportion of mosquitoes which pick up infection when biting an infected host
    double sigma;           //death rate
    
    vectorSpecies species;
    
    int maxRatio;
    int meanRatio;
    
    
    typedef struct {
        
        int year;
        double larvalDensity;
        
    } savedYear;
    
    std::vector<savedYear> savedYears;
    
    
};

#endif /* Vector_hpp */
