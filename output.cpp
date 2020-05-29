/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
//
#include "AIG.h"

output::output(){}

output::~output(){}


void output::pushInput(nodeAig* param, bool param_polarity) {
    //TRUE to invert FALSE to regular
    if(param_polarity)
            param=((nodeAig*)(((uintptr_t)param) ^ 01));
        input=param;
}

nodeAig* output::getInput(){
    return this->input->fixLSB();
}


int output::getInputPolarity(){
    return ((int)((uintptr_t)input) & 01);
}

void output::writeNode(ofstream& write){
    int depth;
    write<<"Output: "<<this->id+this->getInputPolarity()<<". Input:"<<this->getInput()->getId()<<endl;
}

int output::computeDepthInToOut(){
    int depth=0;
    ofstream write;
    if(this->id>1)
        depth=this->getInput()->fixLSB()->computeDepthInToOut();

    return depth;
}

void output::printNode(){
    cout<<"PO:"<<this->id<<". Input:";
        cout<<this->input->fixLSB()->getId()+(int)getThisPtrPolarity(input)<<",";
    cout<<endl;
}
        
unsigned long long int output::PropagSignalDFS(){
    if(this->signal==-1){
        this->bit_vector=this->input->fixLSB()->PropagSignalDFS();
        if(this->getInputPolarity()){
            this->bit_vector=~this->bit_vector;
        }
    }
    return this->bit_vector;
}


void output::clearInput(){
    nodeAig* null_node=NULL;
    this->input=null_node;
}