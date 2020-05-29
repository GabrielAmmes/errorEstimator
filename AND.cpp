/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "AIG.h"


AND::AND(){}

AND::~AND(){}

vector<nodeAig*> AND::getInputs(){
    vector<nodeAig*> ret(2);
    ret[0]=this->inputs[0]->fixLSB();
    ret[1]=this->inputs[1]->fixLSB();
    return ret;
} 



void AND::pushInput(nodeAig* param, bool param_polarity){
    if(this->inputs.size()>=2)
        cout<<"ERROR, trying to add more then 2 inputs on AND node: "<<this->id<<endl;
    else
    {
        if(param_polarity)
        {
            param=((nodeAig*)(((uintptr_t)param) ^ 01));
            
        }
        if(inputs.size()>0)
        {
            if(inputs[0]->fixLSB()->getId()>=param->fixLSB()->getId())
                inputs.push_back(param);
            else
            {
                nodeAig* aux;
                aux=inputs[0];
                inputs[0]=param;
                inputs.push_back(aux);
            }
        }
        else
            inputs.push_back(param);
    }
}

int AND::computeDepthInToOut(){
    if(this->signal==-1)
    {
        int depth1,depth2;
        depth1=this->getInputs()[0]->fixLSB()->computeDepthInToOut();
        depth2=this->getInputs()[1]->fixLSB()->computeDepthInToOut();
        int v;         
        unsigned int r;  
        v=depth1-depth2;
        int const mask = v >> sizeof(int) * CHAR_BIT - 1;

        r = (v + mask) ^ mask;
        signal=(((depth1+depth2)+r)/2) +1 ;
        return signal;
    }
    else 
        return signal;
    
}

void AND::writeNode(ofstream& write){
    int depth;

    write<<"AND: "<<this->id<<" .Inputs("<<this->getInputs().size()<<"):";
    
    write<<this->getInputs()[0]->getId()+this->getInputPolarities()[0];
    write<<",";
    
    write<<this->getInputs()[1]->getId()+this->getInputPolarities()[1];
#if IGNORE_OUTPUTS ==0
    write<<" Outputs: ";
    for(int i=0;i<this->outputs.size();i++)
        write<<this->outputs[i]->getId()<<",";
#endif
    write<<endl;
}


vector<int> AND::getInputPolarities(){
    vector<int> ret(2);
    //returns FALSE for regular and TRUE for inverted
    ret[0]=(int)((uintptr_t)inputs[0]) & 01;
    ret[1]=(int)((uintptr_t)inputs[1]) & 01;
    return ret;
}


void AND::printNode(){
    cout<<"AND:"<<this->id<<". Signal:"<<this->signal<<". Inputs:";
    for(int a=0;a<this->inputs.size();a++)
        cout<<inputs[a]->fixLSB()->getId()+(int)getThisPtrPolarity(inputs[a])<<",";
#if IGNORE_OUTPUTS ==0
    cout<<". Outputs:";
    for(int a=0;a<this->outputs.size();a++)
        cout<<outputs[a]->fixLSB()->getId()+(int)getThisPtrPolarity(outputs[a])<<",";
#endif
    
    cout<<endl;
}

unsigned long long int AND::PropagSignalDFS(){
    unsigned long long int sig_rhs0,sig_rhs1;
    switch (this->signal){
        case -1:
        sig_rhs0=inputs[0]->fixLSB()->PropagSignalDFS();
        sig_rhs1=inputs[1]->fixLSB()->PropagSignalDFS();
        if(getInputPolarities()[0])
            sig_rhs0=~sig_rhs0;
        if(getInputPolarities()[1])
            sig_rhs1=~sig_rhs1;
        this->bit_vector= sig_rhs0 & sig_rhs1;

        this->signal=1;
    }
    return this->bit_vector;

}

void AND::replaceInput(int swap_index,nodeAig* new_node,bool polarity) {
    if(polarity)
        new_node=new_node->forceInvert();
    this->inputs[swap_index]=new_node;
}



#if IGNORE_OUTPUTS ==0
void AND::pushOutput(nodeAig* param){
    this->outputs.push_back(param);
}

vector<nodeAig*> AND::getOutputs(){
    return this->outputs;
}

void AND::clearOutputs() {
    this->outputs.clear();
}

void AND::removeOutput(unsigned int id_to_remove) {
    for(int i=0;i<outputs.size();i++)
    {
        if(outputs[i]->getId()==id_to_remove)
        {
            outputs.erase(outputs.begin()+i);
            break;
        }
    }
}

#endif

void AND::invertInputs() {
        nodeAig* aux;
    if(inputs[0]->fixLSB()->getId()<inputs[1]->fixLSB()->getId())
    {
        aux=inputs[0];
        inputs[0]=inputs[1];
        inputs[1]=aux;
    }
	else if (inputs[0]->fixLSB()->getId()==inputs[1]->fixLSB()->getId())
	{
            if(this->getInputPolarities()[0]==0 && this->getInputPolarities()[1]==1)
            {
                    aux=inputs[0];
                    inputs[0]=inputs[1];
                    inputs[1]=aux;
            }
	}
	
}
