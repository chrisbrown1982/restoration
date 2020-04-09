//
//  Population.cpp
//  transfil
//
//  Created by Paul Brown on 03/03/2017.
//  Copyright © 2017 Paul Brown. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include "Population.hpp"
#include "Worm.hpp"
#include "MDAEvent.hpp"
#include "../../tracing.hpp"
#include "loops.hpp"

extern bool _DEBUG;

extern Statistics stats;

Population::Population(const std::string paramsfile){
    
    //initialise from file
    
    //load csv file
    std::ifstream infile(paramsfile, std::ios_base::in);
    
    if(!infile.is_open()){
        std::cout << "Error in Population::Population. Cannot read file " << paramsfile << std::endl;
        exit(1);
    }
    
    //look for section headed [Vector]
    
    std::string line;
    bool found = false;
    
    while (getline(infile, line)){
        
        
        if (line == "[Host]"){
            
            getline(infile, line);
            tau  = atof(line.c_str());
            getline(infile, line);
            defSysCompBednets  = atof(line.c_str());
            getline(infile, line);
            rhoBU  = atof(line.c_str());
            getline(infile, line);
            rhoCN  = atof(line.c_str());
            getline(infile, line);
            defSysCompMDA  = atof(line.c_str());
            infile >> meanaImp >> maxaImp;
            getline(infile, line);
            infile >> mink >> maxk;
            
            found = true;
            break;
            
        }
        
    }
    
    infile.close();
    
    if(!found){
        std::cout << "Error in Population::Population. File " << paramsfile << " does not contain all the required parameters."<< std::endl;
        exit(1);
    }else
        std::cout << "Population parameters: tau=" << tau << ", sysCompBednets=" << defSysCompBednets << ", rhoBU=" << rhoBU << ", rhoCN=" << rhoCN << ", sysCompMDA=" << defSysCompMDA  << ", k = [" << mink << " " << maxk << "],  aImp = [" << meanaImp << " " << maxaImp << "]" << std::endl << std::endl;
    
    
    //amount of systematic adherance, if 0, no systematic adherence so completely random which individuals comply in each round, if 1 then same individuals comply in each
    
    sysCompMDA = -1; //define this the first time mda applied
    sysCompBednets = -1;
    
    //Variance for calculating u0
  //  sigma2Bednets = sysCompBednets/(1-sysCompBednets);
   // sigma2MDA = sysCompMDA/(1-sysCompMDA);
    
    //setUB need SD
   // sigmaBednets = sqrt(sigma2Bednets);
    //sigmaMDA = sqrt(sigma2MDA);

    //need default values for these two
    sigmaMDA = 0; //might be needed in updatebednetcoverage before drugs actually applied. This removes correleation
    sigmaBednets = sqrt(defSysCompBednets/(1-defSysCompBednets)); //default value needed if mda applied with zero bednets. Cannot be zero or setU will crash
    
    bedNetCov = 0.0;
   
}


double Population::calcU0(double coverage, double sigma2){
    
    //u0 is the mean of the distribution of p values from which individual host p values for MDA/bednet adherence are drawn
    //sigma2 is the variance and determines systematic adherence. High variance means high systematic adherence as p vlaues will be widely distributed
    //Coverage determines the absolute values as the mean of all host p values must be equal to this
    
    return -1*stats.cdf_normal_Pinv(coverage, 1.0) * sqrt(1 + sigma2); //inverse cdf for gaussian distrib with SD=1
    
}


void Population::loadPopulationSize(const std::string popfile){
    
    
    //load csv file
    std::ifstream infile(popfile, std::ios_base::in);
    
    if(!infile.is_open()){
        std::cout << "Error in Population::loadPopulationSize. Cannot read file" << popfile << std::endl;
        exit(1);
    }
    
    //How many lines?
    int numLines = 0;
    std::string line, size, prob;
    
    while(getline(infile, line))
        numLines++;
    
    //read file
    infile.clear();
    infile.seekg(0,std::ios::beg);
    
    popSize = new int[--numLines]; //-1 as header line
    std::vector<double> popWeight; //assumption that col2 in file will sum to 1.0
    popWeight.reserve(numLines);
    
    getline(infile, line);//ignore header
    for(int i = 0; i < numLines; i++){
        
        getline(infile, size, ',');
        getline(infile, prob);
        popSize[i] = atoi(size.c_str());
        popWeight.push_back(atof(prob.c_str()));
        
    }
    
    
    infile.close();
    
    
    if (*std::max_element(popSize, popSize+numLines) > MAX_POP){
        std::cout << "Error in Population::loadPopulationSize. The population size cannot exceed " << MAX_POP << std::endl;
        exit(1);
    }
    
    //set up discrete sampler
    populationDistribution = new std::discrete_distribution<int>(popWeight.begin(), popWeight.end());
    
    //seed random generator
    randgen.seed(0);

    
}

double Population::getPopSize() {
    
    //use distribution defined above to get a population size
    
    unsigned choice =  (*populationDistribution)(randgen);
    //this a number in range 0 to numLines-1
    return popSize[choice];
  
    
}

void  Population::initHosts(std::string distType){
    
    //generate a new population size and reset all members at the start of a new replicate
    size = getPopSize();
    TotalBiteRisk = 0.0;
    
    //new random value for k, shape of gamma distrib
     k = mink + (stats.uniform_dist() * (maxk-mink)); // must be in range 0.01 <= k <= 0.14
    
    //new random value for imporation rate
    if(distType == "uniform")
        aImp_original = (stats.uniform_dist() * maxaImp); //must be in range 0 <= aImp <= 0.002
    else {
      __begin_loop(LOOP183);
      do {
        __begin_iteration(LOOP183);
        aImp_original = stats.exp_dist(meanaImp);
        __end_iteration(LOOP183);
      } while (aImp_original > maxaImp);
      __end_loop(LOOP183);
    }
    
    //tmp fix here
    
    aImp = aImp_original; //save this as it need to appear in output file
    
    for(int i =0; i < size; i++) //TotalBiteRisk sums bites per month for whole popln
        host_pop[i].initialise(tau, maxAge, k, &TotalBiteRisk); //sets worm count to 0 and treated/bednet to 0. Sets random age and bite risk
    
    sysCompMDA = -1; //define this the first time mda applied
    sysCompBednets = -1;
    sigmaMDA = 0; //might be needed in updatebednetcoverage before drugs actually applied. This removes correleation
    bedNetCov = 0.0;
    
}

double Population::getPrevalence() const {
    
    int prev = 0;
    for(int i =0; i < size; i++)
        prev += ( stats.uniform_dist() <  (1 - exp(-1 * host_pop[i].M) ) ); 
    
    return (double) prev / (double) size;
    
}

double Population::getLarvalUptakebyVector(double r1, double kappas1, Vector::vectorSpecies species) const {
    
    //this is the rate at which the vector population takes up l3 larvae from each host
    //return total uptake for whole population weighted according to each hosts's bite risk
    
    double mf = 0.0;
    double uptake;
    
    __begin_loop(LOOP217);
    for(int i =0; i < size; i++){
        __begin_iteration(LOOP217);
        
        uptake = ( 1 - exp(-r1 * host_pop[i].M /kappas1) );
        if (species==Vector::Anopheles) uptake *= uptake;
        mf += host_pop[i].biteRisk * kappas1 * uptake;
        
        __end_iteration(LOOP217);
    }
    __end_loop(LOOP217);
    
    return mf/TotalBiteRisk;

    
}

void Population::evolve(double dt, const Vector& vectors, const Worm& worms){
    
    //advance one time step
    __begin_loop(LOOP233);
    for(int i =0; i < size; i++) {
        __begin_iteration(LOOP233);
        host_pop[i].react(dt, tau, maxAge, aImp,  vectors, worms);
        __end_iteration(LOOP233);
    }
    __end_loop(LOOP233);
    
    
}

int Population::getMaxAge() const {
    
    return maxAge;
    
}

void Population::saveCurrentState(int year, std::string sname){
    
    //save host states and importation rate and year
    
    savedYear currentState;
    
    currentState.scenario = sname; //debugging only
    currentState.data.resize(size, {0, 0, 0.0, 0.0, 0, 0}); //WM, WF,M,biteRisk,age,monthSinceTreated
    
    for(int i =0; i < size; i++)
        currentState.data[i] = host_pop[i]; //overloaded operator extracts data members to struct
    
    
    currentState.year = year;
    
    currentState.aImp = aImp;
    currentState.sysCompMDA = sysCompMDA;
    currentState.sysCompBednets = sysCompBednets;
    
    savedYears.push_back(currentState);
    
}

void Population::resetToYear(int year){
    
    //Will always discard any years coming after the one required
    

    
    while(savedYears.size()){
        
        savedYear lastYear = savedYears.back();
        
        if (lastYear.year == year){
            
            //restore host state and importation rate
            for(int i =0; i < size; i++)
                host_pop[i].restore(lastYear.data[i]);
                
            aImp = lastYear.aImp;
            
            sysCompMDA = lastYear.sysCompMDA;
            sysCompBednets = lastYear.sysCompBednets;


            if(_DEBUG) std::cout << "Reset to year " << year << ", that was saved by " << lastYear.scenario << std::endl;

            return;
        }
        savedYears.pop_back();

    }
    
    std::cout << "Error in Population::resetToYear. Cannot find the specified year " << year << std::endl;
    exit(1);
    
    
}



void Population::updateBedNetCoverage(double cov, double sysComp){
    
    //called at the start of each new year as coverage changes
    
    //which individuals have a bednet affects host::react()
    //proportion covered affects Vector::larvalDensity
    
    //if sysComp has a valid value [0...1] then systematic adherence has changed so need to recaculate u0CompBednets

    //This function affects host.uCompMDA via call to setUB!!!! if sigmaMDA > 0, different random noise applied
    

    bool sysCompChanged = (sysComp >= 0);
    
    if (!cov){ //!cov can't be zero or u0CompBednets is infinite
        
        bedNetCov = 0;
        for(int i =0; i < size; i++)
            host_pop[i].bedNet = 0;
        
    }else if (cov > 0){
        
        if (!bedNetCov){
            
            //first time bednets applied so need to generate u0
            // no, need to recall previuos value
            if(sysComp < 0) //use default
                sysCompBednets = defSysCompBednets;
                
            
            sigma2Bednets = sysCompBednets/(1-sysCompBednets);
            sigmaBednets = sqrt(sigma2Bednets);
            u0CompBednets = calcU0(cov, sigma2Bednets);
            
            
            if(!sigmaMDA){
                //This means each indivual has an equal chance of complying each round.
                for(int i =0; i < size; i++)
                    host_pop[i].uCompBednets = stats.normal_dist(u0CompBednets,sigmaBednets);
            
            }else if (!sigmaBednets){
                for(int i =0; i < size; i++)
                    host_pop[i].uCompBednets = u0CompBednets;
                
            }else
                for(int i =0; i < size; i++) //alters MDA compliance too. Maybe don't do this if not in an mda period
                    setU(host_pop[i], sigmaMDA, sigmaBednets); //crashes if either input is zero
            
        }
        
        
        if(!sysCompChanged){
            
            //systematic adherence is not changing so just need to scale original u0 value for new coverage
            //Just scaling it means the same hosts have high and the same have low u values, though absolute values
            //change to reflect new coverage
        
            double scalefactor = calcU0(cov, sigma2Bednets)-u0CompBednets; //compared to original value at start of replicate
        
            //An individuals probability changes but relative values stay the same
            for(int i =0; i < size; i++)
                host_pop[i].bedNet = (stats.normal_dist(host_pop[i].uCompBednets+scalefactor,1.0)<0)? 1:0;
        
            //a random sample of hosts should now have a bednet, with proportion of cov
        
        }else{
            //need to check for change
            //systematic adherence value has changed so we must re-calculate u0CompBednets to reflect new variance and coverage
            
            sigma2Bednets = sysComp/(1-sysComp);
            sigmaBednets = sqrt(sigma2Bednets);
            u0CompBednets = calcU0(bedNetCov, sigma2Bednets);
            
            for(int i =0; i < size; i++){
                setUB(host_pop[i]); //uses sigmaBednets and u0CompBednets to set Host.uCompBednets
                host_pop[i].bedNet = (stats.normal_dist(host_pop[i].uCompBednets,1.0)<0)? 1:0;
            }
            
        }
        
    }
    
    bedNetCov = cov;
        
    if(_DEBUG){
             int nets=0;
            for(int i =0; i < size; i++)
                if (host_pop[i].bedNet) nets++;
          std::cout << "specified coverage = " << bedNetCov << ", actual coverage = " << double(nets)/double(size) << std::endl;
    }
    
       
}

void Population::ApplyTreatment(MDAEvent& mda, Worm& worms) {
    
    //also must check if syscomp has changed since last scenario!!
    //coverage could change too so meed scalefactor
    
    double cov = mda.getCoverage();
    if (!cov) return; //crashes if this is zero, but never should be
    
    bool sysCompChanged = false, firstRound = false;
    double covScaleFactor = 0.0;
    
    if((firstRound = (sysCompMDA < 0))){ //first time since t=0
        //mdaevent specified compliance,or use default
        sysCompMDA = (mda.getCompliance()>=0)? mda.getCompliance():defSysCompMDA;
        
    }else{
        //compliance may have changed if this is a new scenario with treatment continuing from  a previous scenario
        //mda may specify a new value or use default
        double newSysComp = (mda.getCompliance()>=0)? mda.getCompliance():defSysCompMDA;
        if ((sysCompChanged = (newSysComp != sysCompMDA)))
            sysCompMDA = newSysComp;
        
    }
    
    if(mda.getCompliance() < 0)
        mda.setCompliance(sysCompMDA);  //just ensures right vlaue written to results file
    
    if (firstRound || sysCompChanged){
        
        //need to create distribution from which hosts p values will be drawn. THis will have mean u0, which depends on coverage and a variance which depends on sysComp. High variance means highly systematic compliance
            
        sigma2MDA = sysCompMDA/(1-sysCompMDA);
        sigmaMDA = sqrt(sigma2MDA);
        mdaCoverage = cov; //note coverage used to get u0
        u0CompMDA = calcU0(mdaCoverage, sigma2MDA);
        
        if(!sigmaMDA){
            //SD is zero. This means each indivual has an equal chance of complying each round.
            for(int i =0; i < size; i++)
                host_pop[i].uCompMDA = u0CompMDA; // MDA value fixed for all hosts, ie no systematic adherence
            
        }else{
            
            for(int i =0; i < size; i++)
                setU(host_pop[i], sigmaMDA, sigmaBednets); //uses sigmaMDA and u0CompMDA to set Host.uCompMDA
            
            //The call to setU affects bednet coverage as sigmaMDA > 0 indicates the two are corellated.
            //It will have changed hosts uCompBednets value
            //Must re-apply bednets to reflect this even though bednet coverage won't have changed, different hosts may have complied
            updateBedNetCoverage(bedNetCov);
        }
        
    }else{
        
        //systematic adherence not changing, no need to recalculate u0, so we can keep saem individuals with high/low u values
        //Just need to scale them to reflect new coverage if this has changed
        //mdaCoverage is the vlaue that was used to get current u0

        if(cov != mdaCoverage)
            covScaleFactor = calcU0(cov, sigma2MDA)-u0CompMDA; //default = 0
        
    }

    
    __begin_loop(LOOP464);
    for(int i =0; i < size; i++) {
        __begin_iteration(LOOP464);
        if (stats.normal_dist(host_pop[i].uCompMDA+covScaleFactor,1.0)<0) host_pop[i].getsTreated(worms, mda.getType());
        __end_iteration(LOOP464);
    }
    __end_loop(LOOP464);
    
}


double Population::getBedNetCoverage() const {
    
    return bedNetCov;
    
}

void Population::updateImportationRate(double factor) {
    
    aImp *= factor;
}



void Population::clearSavedYears(){
    
     savedYears.clear();
    
}

//print random variables

std::ostream& operator<<(std::ostream& ostr, const Population& pop){
    
    return ostr << pop.size << "\t" << pop.k << "\t" << pop.aImp_original;
    
}


//These 2 functio nreturn that names and values to be printed to file in the order they are listed. Must correspond with each other!

std::vector<std::string> Population::printRandomVariableNames() const {
    
    //outputs a list of name/value pairs of any length. These are passed to output object to be printed to file
    
    std::vector<std::string> names = {"Population", "k", "aImp"};
    return names;
    
}

std::vector<double> Population::printRandomVariableValues() const {
    
    //outputs a list of name/value pairs of any length. These are passed to output object to be printed to file
    
    std::vector<double> values = {static_cast<double>(size), k, aImp_original};
    return values;
    
}


void Population::setU(Host& h, double sigmaMDA, double sigmaBednets){
    
    //create correlation matrix and mean for multivariate normal distribution.
    
    gsl_matrix * m = gsl_matrix_alloc (3, 3);
    gsl_matrix_set (m, 0, 0, 1.0);
    gsl_matrix_set (m, 0, 1, sigmaMDA * rhoBU);
    gsl_matrix_set (m, 1, 0, sigmaMDA * rhoBU);
    gsl_matrix_set (m, 1, 1, sigmaMDA*sigmaMDA);
    gsl_matrix_set (m, 2, 0, sigmaMDA * rhoCN * sigmaBednets);
    gsl_matrix_set (m, 0, 2, sigmaMDA * rhoCN * sigmaBednets);
    gsl_matrix_set (m, 2, 1, 0.0);
    gsl_matrix_set (m, 1, 2, 0.0);
    gsl_matrix_set (m, 2, 2, sigmaBednets*sigmaBednets);
    
    //set mean vector
    gsl_vector * v = gsl_vector_alloc (3);
    gsl_vector_set (v, 0, 0.0);
    gsl_vector_set(v,1, u0CompMDA);
    gsl_vector_set(v,2, u0CompBednets);
        
    //set result vector
    gsl_vector * result = gsl_vector_alloc (3);
    rmvnorm(3, v, m, result);
        
        
    h.uCompMDA = (double) gsl_vector_get (result, 1);
    h.uCompBednets = (double) gsl_vector_get (result, 2);
        
    gsl_matrix_free (m);
    gsl_vector_free (v);
    
}



void Population::setUB(Host& h){
    
    //create correlation matrix and mean for multivariate normal distribution.
    
    //u - individual p value for MDA compliance
    //n - same for bednets
    
    if (!sigmaMDA){ //SD is zero. This means each indivual has an equal chance of complying each round.
        
        h.uCompMDA = u0CompMDA; // MDA value fixed for all hosts, ie no systematic adherence
        h.uCompBednets = stats.normal_dist(u0CompBednets,sigmaBednets); //u0-mean, sigCompN - SD, n will also be u0CompBednets if sigCompN is zero;
        
    }else{
        
        gsl_matrix * m = gsl_matrix_alloc (3, 3);
        gsl_matrix_set (m, 0, 0, 1.0);
        gsl_matrix_set (m, 0, 1, sigmaMDA * rhoBU);
        gsl_matrix_set (m, 1, 0, sigmaMDA * rhoBU);
        gsl_matrix_set (m, 1, 1, sigma2MDA);
        gsl_matrix_set (m, 2, 0, sigmaMDA * rhoCN * sigmaBednets);
        gsl_matrix_set (m, 0, 2, sigmaMDA * rhoCN * sigmaBednets);
        gsl_matrix_set (m, 2, 1, 0.0);
        gsl_matrix_set (m, 1, 2, 0.0);
        gsl_matrix_set (m, 2, 2, sigma2Bednets);
        
        
        //set mean vector
        gsl_vector * v = gsl_vector_alloc (3);
        gsl_vector_set (v, 0, 0.0);
        gsl_vector_set(v,1, u0CompMDA);
        gsl_vector_set(v,2, u0CompBednets);
        
        //set result vector
        gsl_vector * result = gsl_vector_alloc (3);
        rmvnorm(3, v, m, result);
        
       
        h.uCompMDA = (double) gsl_vector_get (result, 1);
        h.uCompBednets = (double) gsl_vector_get (result, 2);
        
        gsl_matrix_free (m);
        gsl_vector_free (v);
        

    }
    
}

void Population::rmvnorm(const int n, const gsl_vector *mean, const gsl_matrix *var, gsl_vector *result){
    /* multivariate normal distribution random number generator */
    /*
     * n dimension of the random vetor
     * mean  vector of means of size n
     * var variance matrix of dimension n x n
     * result  output variable with a sigle random vector normal distribution generation
     */
    
    int k;
    gsl_matrix *work = gsl_matrix_alloc(n,n);
    
    gsl_matrix_memcpy(work,var);
    gsl_linalg_cholesky_decomp(work); //decompose variance
    
    for(k=0; k<n; k++) //randomise result
        gsl_vector_set( result, k, stats.unit_normal_dist());
    
    gsl_blas_dtrmv(CblasLower, CblasNoTrans, CblasNonUnit, work, result); //result = result + A, A'
    gsl_vector_add(result,mean); //result = var*norm_distrib+mean
    
    gsl_matrix_free(work);
    
}




