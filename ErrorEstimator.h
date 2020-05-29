#ifndef ERRORESTIMATOR_H
#define ERRORESTIMATOR_H

#include <vector>
#include "AIG.h"
using namespace std;

class ErrorEstimator {
    //aigraph graph_orig;
    vector<vector <unsigned long long>> sim_vectors,out_orig;
    vector<double> errors;
    unsigned nVectors, nInput, nOutput;

public:
    ErrorEstimator(aigraph &graph_obj, unsigned nVectors);
    ErrorEstimator(const ErrorEstimator& errorEstimator);
    virtual ~ErrorEstimator();
    void clearErrorEstimator();
    
    vector<vector <unsigned long long>> getSimVectors();
    vector<vector <unsigned long long>> getOutOrig();
    //aigraph* getGraphOrig();
    unsigned getWBF();
    double getER();
    double getMAE();
    double getMSE();
    double getMRE();
    unsigned long long getWCE();
    double getWCRE();
    void computeError(vector<vector <unsigned long long>> out_app);
    void computeError(aigraph &Sgraph_app);

private:
    void generateSimVectors();
    void clearError();

};

#endif

