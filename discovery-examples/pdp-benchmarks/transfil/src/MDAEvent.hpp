//
//  MDAEvent.hpp
//  transfil
//
//  Created by Paul Brown on 12/05/2017.
//  Copyright © 2017 Paul Brown. All rights reserved.
//

#ifndef MDAEvent_hpp
#define MDAEvent_hpp

#include <climits>
#include <string>

class MDAEvent {
    
    
public:
    
    MDAEvent(): startYear(INT_MAX), mdaFreq(INT_MAX), nMDA(0), covMDA(0.0), type(std::string("")), sysComp(-1), numDone(0) {
        
    };
    
    MDAEvent(int yr, int freq, int num, double cov, std::string t, double sysC) : startYear(yr), mdaFreq(freq), nMDA(num), covMDA(cov), type(t), sysComp(sysC), numDone(0) {};
    
    MDAEvent(int yr, int freq, int num, double cov, std::string t) : startYear(yr), mdaFreq(freq), nMDA(num), covMDA(cov), type(t), sysComp(-1), numDone(0) {
    
    };
    
    int getStartYear() const;
    double getCoverage() const;
    std::string getType() const;
    double getCompliance() const;
    bool treatmentDue(int year, int month);
    void reset();
    void setCompliance(double sysComp);
    
    
    
private:
    
    bool finished() const;

    int startYear;
    int mdaFreq;   //frequency, every freq months
    int nMDA;    //numbe rof rounds
    double covMDA; //coverage 0 - 1
    
    std::string type;
    double sysComp;
    
    double numDone;
    
    
    
};

#endif /* MDAEvent_hpp */
