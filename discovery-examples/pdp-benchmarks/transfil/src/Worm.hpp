//
//  Worm.hpp
//  transfil
//
//  Created by Paul Brown on 08/03/2017.
//  Copyright © 2017 Paul Brown. All rights reserved.
//

#ifndef Worm_hpp
#define Worm_hpp
#include <string>


class Worm {
    
    //class tomodel worm dynamics in host and vector
    
public:
    
    Worm(const std::string paramsfile);
    double proportionPerBite() const;
    double getDeathRate() const;
    double getMFDeathRate() const;
    double repRate(unsigned monthsSinceTreated, int femaleWorms, int maleWorms) const;
    double getPropLeavingVectorPerBite() const;
    int wormsTreated(int W, std::string type);
    double mfTreated(double M, std::string type) ;
    
    
private:
    
    double psi1;    //Proportion of L3 leaving mosquito per bite
    double psi2;    //Proportion of L3 leaving mosquito that enter host
    double s2;      //Prop of L3 that enter human host developing into adult.
    double mu;      //worm death rate
    double gamma;   //mf death rate
    double alpha;   //mf birth rate per female worm
    double nu;      //poly-monogamy parameter. If zero, reproduction is proportional to num females as long as at least one male present.
                    //Otherwise, larger values make it more likely rate will be lower and proportional to num males
    double mfPropMDA;   //proportion mf killed by drugs
    double wPropMDA;    //proportion worms killed by drugs
    double fecRed;      //WF can't produce more mf for this many months after MDA
    
    
    
};

#endif /* Worm_hpp */
