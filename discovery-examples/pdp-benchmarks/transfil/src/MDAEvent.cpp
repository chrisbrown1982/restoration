//
//  MDAEvent.cpp
//  transfil
//
//  Created by Paul Brown on 12/05/2017.
//  Copyright © 2017 Paul Brown. All rights reserved.
//

#include "MDAEvent.hpp"


double MDAEvent::getCoverage() const{
    
    return covMDA;
}

std::string MDAEvent::getType() const{
    
    return type;
}

double MDAEvent::getCompliance() const {
    
    return sysComp;
}


bool MDAEvent::finished() const {
    
    return numDone >= nMDA;
}

int MDAEvent::getStartYear() const{
    
    return startYear;
}

bool MDAEvent::treatmentDue(int year, int month) {
    
    if ((year >= startYear) && !(month % mdaFreq) && !finished()){
        //start year has passed but not all rounds given
        numDone++;
        return true;
    }else
        return false;
    
}

void MDAEvent::reset(){
    
    numDone = 0; //start of new replicate
}

void MDAEvent::setCompliance(double s){
    
    sysComp = s;
}

