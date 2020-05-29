/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   AIG.h
 * Author: augusto
 *
 * Created on 23 de Março de 2018, 16:56
 */

#ifndef GDE_H
#define GDE_H

#define DBGVAR( os, var ) \
  (os) << "DBG: " << __FILE__ << "(" << __LINE__ << ") "\
       << #var << " = [" << (var) << "]" << std::endl

#define PROBS_FROM_FILE 1
#define IGNORE_OUTPUTS 0
#define SIMPLIFIEDAIG 1
#define DEBUG 0
#define debug_value 3

#define TEST 0
#define RENUMBER 1
#define REMOVE_PI 0
#define USE_ABC 0
#define FIX_DOUBLED_NODES 1
#define EXECUTE_ONCE 0
#define APPLY_MNIST 0
#define CEC 0
#define WRITE_AIG 0
#define WRITE_AAG 0
#define WRITE_ORIGINAL_DEPTHS 0



#define posY_max 28
#define posX_max 224

#define BITS_PACKAGE_SIZE 64

#include <vector>
#include <unordered_set>
#include <array>
#include <set>
#include <deque>
#include <stack>
#include <map>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <limits.h> //char_bit
#include <algorithm> //max
#include <stdint.h> //uintptr_t
#include <stdlib.h>
#include <functional> //reference_wraper
#include <sys/time.h>
#include <math.h> //pow
#include <sys/resource.h> //getrusage
#include <string.h> //strlen, strncmp
#include <bitset> //check bits on bit-parallel simulation
#include <random>

using namespace std;









class nodeAig{
    
protected:
    unsigned int id;
    int signal;
    unsigned long long int bit_vector;
    
public:
    nodeAig();
    nodeAig(unsigned int);
    virtual ~nodeAig();
    
    //modifiers
    virtual void pushOutput(nodeAig* param){}
    virtual void pushInput(nodeAig* param,bool param_polarity){}
    void setId(unsigned int);
    void setDepth(int);
    void setSignal(int);
    void setBitVector(unsigned long long int);
    
    //member access
    unsigned int getId();
    int getDepth();
    int getSignal();
    unsigned long long int getBitVector();
    nodeAig* fixLSB();    
    nodeAig* forceInvert();
    virtual vector<nodeAig*> getInputs(){}
    virtual vector<nodeAig*> getOutputs(){}
    virtual vector<int> getInputPolarities(){}
    
    
    //operations
    virtual int computeDepthInToOut(){}
    virtual unsigned long long int PropagSignalDFS(){}
    virtual void replaceInput(int swap_index,nodeAig* new_node,bool polarity){}
    virtual void removeOutput(unsigned int){}
    virtual void clearOutputs(){}
    virtual void writeNode(ofstream&){}
    virtual void printNode(){}
};








class input: public nodeAig {
    vector<nodeAig*> outputs;
    
public:
    input();
    using nodeAig::nodeAig; //enable use of constructor from node
    using nodeAig::setDepth;
    using nodeAig::getDepth;
    using nodeAig::getId;
    using nodeAig::getSignal;
   virtual ~input();
   
   //modifiers
   void pushOutput(nodeAig* param) override;
   
   //operations
   int computeDepthInToOut() override;
   unsigned long long int  PropagSignalDFS() override;
   void removeOutput(unsigned int id_to_remove) override;
   void clearOutputs() override;
   
   

   void writeNode(ofstream&) override;
   void printNode() override;
   //member access
   vector<nodeAig*> getOutputs() override;
   

   
};








class latch : public nodeAig{
    nodeAig* input;
    vector <nodeAig*> outputs;
public:
    //constructor
    latch();
    using nodeAig::nodeAig; //enable use of constructor from node
    virtual ~latch();
    
    //member access
    using nodeAig::getDepth;
    using nodeAig::getId;
    using nodeAig::getSignal;
    nodeAig* getInput();
    
    //modifiers
    using nodeAig::setDepth;
    void pushInput(nodeAig* param,bool param_polarity) override; 
    void pushOutput(nodeAig* param) override;

    //operations    
    int computeDepthInToOut() override;
    
};








class output : public nodeAig{
    nodeAig* input;
    
public:
    output();
    using nodeAig::nodeAig; //enable use of constructor from node
    virtual ~output();
    using nodeAig::setDepth;
    using nodeAig::getDepth;
    using nodeAig::getSignal;
    
    //modifiers
    void pushInput(nodeAig* param,bool param_polarity);
    void clearInput();
            
    //member access
    nodeAig* getInput();
    int getInputPolarity();
    
    //operations
    void writeNode(ofstream&) override;
    int computeDepthInToOut() override;
    void printNode() override;
    unsigned long long int PropagSignalDFS() override;

};



class AND : public nodeAig{
    vector <nodeAig*> inputs;
#if IGNORE_OUTPUTS == 0
    vector <nodeAig*> outputs;
#endif
    
public:
    AND();
    using nodeAig::nodeAig; 
    using nodeAig::setDepth;
    using nodeAig::getDepth;
    using nodeAig::getSignal;
    virtual ~AND();
    
    //member access
    vector<nodeAig*> getInputs() override;
    vector<nodeAig*> getOutputs() override;

   vector<int> getInputPolarities() override;
   
   //modifiers
   
   void pushInput(nodeAig* param,bool param_polarity) override;
   void replaceInput(int swap_index,nodeAig* new_node,bool polarity) override;
   void invertInputs();
   
   //operations
   int computeDepthInToOut() override;
   unsigned long long int PropagSignalDFS() override;
   
   void removeOutput(unsigned int id_to_remove) override;
   void clearOutputs() override;
   void pushOutput(nodeAig* param) override;

   
   
   void writeNode(ofstream&) override;
   void printNode() override;

};






class aigraph {
protected:
    string name; //ReadAIG
    //Original order of Primary Outputs
    vector<unsigned int> POs_order;//ReadAIG
    //all_depths' index is the node ID/2 and the all_depths[ID/2] is the depth for this node.
    vector<unsigned int> all_depths;
    int graph_depth;
    vector<unsigned int> greatest_depths_ids;
    nodeAig constant1,constant0;
    vector<input> all_inputs; //ReadAIG
    vector<latch> all_latches; //ReadAIG
    vector<output> all_outputs; //ReadAIG
    vector<AND> all_ANDS;
    
    ofstream log;
    
public:
    aigraph();
   virtual ~aigraph();
    
    
    //modifiers
    void readAAG(ifstream&,string);
    void readAIG(ifstream&,string);
    void setName(string);
    void clearCircuit();
    void propagSignal(vector<unsigned long long> vectors);
    void renameNodes(vector<vector<unsigned>> &ands);


    unsigned getIndex(unsigned id, unsigned A);
    nodeAig* findAny(unsigned param, unsigned A);
    vector<input>* getInputs();
    vector<latch>* getLatches();
    vector<output>* getOutputs();
    vector<AND>* getANDS();
        
    //member access


    string getName();
    int getDepth();
    
    //operations
    void setDepthsInToOut();
    void setShortestDistanceToPO();
    void writeAAG();
    void writeAIG();
    

	void setInputValue(vector<unsigned long long> vectors);
	void initializeSignals();
    vector<vector <unsigned long long>> simulate(vector<vector<unsigned long long>> sim_vectors, unsigned nVectors);
    vector<unsigned long long> calcOutputValue(unsigned vSize);
    
    
    //debugging
    void printCircuit();
    void printDepths();
    void writeCircuitDebug();
    void printAllNodesBitVector(int nVectors);
    void printBit(unsigned long long vector, int size);
    
private:
    void encodeToFile(ofstream& file, unsigned x);
    unsigned char getnoneofch (ifstream& file,int);
    unsigned decode (ifstream& file,int);
    
    
};


/*struct aux_struct{
    vector<nodeAig*> outputs;
    vector<bool> firsts_polarity;
};*/

//returns a word from a phrase, words are expected to be separeted by any number of blank spaces
string wordSelector(string line, int word_index);

//checks the pointer address passed as reference if it is inverted or not.
bool getThisPtrPolarity(nodeAig* param);

int binToDec(vector<int> param);

#endif /* GDE_H */

