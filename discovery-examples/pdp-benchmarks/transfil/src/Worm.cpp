//
//  Worm.cpp
//  transfil
//
//  Created by Paul Brown on 08/03/2017.
//  Copyright 2017 Paul Brown. All rights reserved.
//


#include <iostream>
#include <fstream>
#include "Worm.hpp"


Worm::Worm(const std::string paramsfile){
    
    
    //initialise from file
    
    //load csv file
    std::ifstream infile(paramsfile, std::ios_base::in);
    
    if(!infile.is_open()){
        std::cout << "Error in Worm::Worm. Cannot read file " << paramsfile << std::endl;
        exit(1);
    }
    
    //look for section headed [Vector]
    
    std::string line;
    bool found = false;
    
    while (getline(infile, line)){
        
        
        if (line == "[Worm]"){
            
            getline(infile, line);
            nu  = atof(line.c_str());
            getline(infile, line);
            alpha  = atof(line.c_str());
            getline(infile, line);
            fecRed  = atoi(line.c_str());
            getline(infile, line);
            psi1  = atof(line.c_str());
            getline(infile, line);
            psi2  = atof(line.c_str());
            getline(infile, line);
            s2  = atof(line.c_str());
            getline(infile, line);
            mu  = atof(line.c_str());
            getline(infile, line);
            gamma  = atof(line.c_str());
            getline(infile, line);
            mfPropMDA  = 1-atof(line.c_str());
            getline(infile, line);
            wPropMDA  = 1-atof(line.c_str());
            found = true;
            break;
            
        }
        
    }
    
    infile.close();
    
    if(!found){
        std::cout << "Error in Worm::Worm. File " << paramsfile << " does not contain all the required parameters."<< std::endl;
        exit(1);
    }else
        std::cout << "Worm parameters: nu=" << nu << ", alpha=" << alpha << ", fecRed=" << fecRed << ", psi1=" << psi1 << ", psi2=" << psi2 << ", s2=" << s2 << ", mu=" << mu << ", gamma=" << gamma << ", mfPropMDA=" << mfPropMDA << ", wPropMDA=" << wPropMDA << std::endl<< std::endl;
    
    
}

double Worm::proportionPerBite() const {
    
   //relates to the number of adult worms acquired by the host per bite
    //product of prop larave leaving vector per bite, prop of these entering host and prop of these reaching adulthood
    //Multiply this by larval density to get worms per bite
    
    return psi1 * psi2 * s2;
    
}


double Worm::getDeathRate() const {
    
    return mu;
}

double Worm::getMFDeathRate() const {
    
    return gamma;
}

double Worm::getPropLeavingVectorPerBite() const {
    
    return psi1;
}


double Worm::repRate(unsigned monthsSinceTreated, int femaleWorms, int maleWorms) const { //reproductive rate of worms in host
    
    if (monthsSinceTreated < fecRed)
        return 0.0; //no mf births in presence of drugs
    
    else if (nu == 0) //polygamous so if at least one male, rate is prop to num females, otherwise zero
        return (maleWorms)? alpha*(double)femaleWorms : 0.0;
    
    else  //monogamous so prop to num males if fewer of these
        return alpha * std::min((double) femaleWorms ,(double) maleWorms / nu);
}



int Worm::wormsTreated(int W, std::string type) {
    
    if (type=="da"){
        wPropMDA = 1-0.55;
        fecRed = 6;
    }else if (type=="ida"){
        wPropMDA = 1-1; //0.55 but the rest are sterilized (=to killed)
        fecRed = 0; //no temporary sterilization
    }else if (type=="aa"){
        wPropMDA = 1-0.55;
        fecRed = 0;
    }else if (type=="ia"){
        wPropMDA = 1-0.35;
        fecRed = 9;
    }else if (type=="ds"){
        wPropMDA = 1-0.59;
        fecRed = 10;
    }else{
        std::cout << " Error in Worm::wormsTreated. Unknown treatment type " << type << std::endl;
        exit(1);
    }
    
    return int(wPropMDA * double(W));
    
}

double Worm::mfTreated(double M, std::string type) {
    
    if (type=="da"){
        mfPropMDA = 1-0.95;
        fecRed = 6;
    }else if (type=="ida"){
        mfPropMDA = 1-1;
        fecRed = 0; //no temporary sterilization
    }else if (type=="aa"){
        mfPropMDA = 1-0;
        fecRed = 0;
    }else if (type=="ia"){
        mfPropMDA = 1-0.99;
        fecRed = 9;
    }else if (type=="ds"){
        mfPropMDA = 1-0.86;
        fecRed = 10;
    }else{
        std::cout << " Error in Worm::wormsTreated. Unknown treatment type " << type << std::endl;
        exit(1);
    }
    
    
    return M * mfPropMDA;
}

















