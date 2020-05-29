/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   input.cpp
 * Author: augusto
 * 
 * Created on 23 de Março de 2018, 17:38
 */

#include "AIG.h"

input::input() {
}



input::~input() {
}

void input::pushOutput(nodeAig* param){
    this->outputs.push_back(param);
}

int input::computeDepthInToOut(){
    int depth;
    ofstream write;
    depth=0;
    this->signal=0;
    return 0; //inputs always return 0 when computing the circuit depth
}



vector<nodeAig*> input::getOutputs(){
    return this->outputs;
}


//AQUI O MAP PODE DAR RUIM
void input::writeNode(ofstream& write){
    int depth;
    
    write<<"Input: "<<this->id<<" .Outputs("<<this->outputs.size()<<"):";
    for(int i=0;i<this->outputs.size();i++)
        write<<this->outputs[i]->getId()<<",";
    write<<endl;
}




unsigned long long int input::PropagSignalDFS(){
    return this->bit_vector;
}

void input::clearOutputs() {
    this->outputs.clear();
}


//AQUI O MAP PODE DAR RUIM
void input::removeOutput(unsigned int id_to_remove) {
    for(int i=0;i<outputs.size();i++)
    {
        if(outputs[i]->getId()==id_to_remove)
        {
            outputs.erase(outputs.begin()+i);
            break;
        }
    }
}


void input::printNode(){
    cout<<"Input:"<<this->id<<". Outputs:";
    
    for(int i=0;i<this->outputs.size();i++)
        cout<<this->outputs[i]->getId()<<",";
    cout<<endl;
}