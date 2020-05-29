/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "AIG.h"

latch::latch(){}

latch::~latch(){}


void latch::pushInput(nodeAig* param,bool param_polarity){
    //TRUE to invert FALSE to regular
    if(param_polarity)
        param=((nodeAig*)(((uintptr_t)param) ^ 01));
    input=param;
}

void latch::pushOutput(nodeAig* param){
    this->outputs.push_back(param);
}



int latch::computeDepthInToOut(){
    int depth;
    ofstream write;

    if(depth==-1)
    {
        depth= ((nodeAig*)input)->computeDepthInToOut() +1 ;

        return depth;
    }
    else 
        return depth;
    
}


nodeAig* latch::getInput(){
    return this->input->fixLSB();
}