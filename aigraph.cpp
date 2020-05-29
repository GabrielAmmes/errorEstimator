/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   circuit.cpp
 * Author: augusto
 * 
 * Created on 23 de Março de 2018, 16:59
 */
 
#include <bits/c++config.h>

#include "AIG.h"

aigraph::aigraph(){
    this->constant1.setId(1);
    this->constant1.setSignal(1);
    this->constant1.setBitVector(ULLONG_MAX);
    this->constant0.setId(0);
    this->constant0.setSignal(1);
    this->constant0.setBitVector(0);
}

aigraph::~aigraph(){}

void aigraph::setName(string param){
    this->name=param;
}

string aigraph::getName(){
    return this->name;
}

int aigraph::getDepth(){
    return this->graph_depth;
}

vector<input>* aigraph::getInputs(){ 
    return &this->all_inputs;
}
vector<latch>* aigraph::getLatches(){
    return &this->all_latches;
}
vector<output>* aigraph::getOutputs(){
    return &this->all_outputs;
}
vector<AND>* aigraph::getANDS(){
    return &this->all_ANDS;
}

void aigraph::clearCircuit(){
    this->all_ANDS.clear();
    this->all_inputs.clear();
    this->all_latches.clear();
    this->all_outputs.clear();
    this->name.clear();
    this->POs_order.clear();
    this->log.close();
    this->all_depths.clear();
    this->greatest_depths_ids.clear();
    this->graph_depth=0;
}

void aigraph::writeAAG(){
    ofstream write;
    int M;
    M=this->all_inputs.size()+this->all_latches.size()+this->all_ANDS.size();
    write.open((this->name+".aag").c_str(),ios::out|ios::trunc);
    write<<"aag "<<M<<" "<<this->all_inputs.size()<<" "<<this->all_latches.size()<<" "<<this->all_outputs.size()<<" "<<this->all_ANDS.size()<<endl;
    
    //write the circuit inputs
    for(vector<input>::iterator it1=this->all_inputs.begin();it1!=this->all_inputs.end();it1++)
        write<<it1->getId()<<endl;

    //write outputs
    for(vector<output>::iterator it2=this->all_outputs.begin();it2!=this->all_outputs.end();it2++)
        write<<it2->getId()+((int)it2->getInputPolarity())<<endl;
    
    //write ANDs
    for(vector<AND>::iterator it3=this->all_ANDS.begin();it3!=this->all_ANDS.end();it3++)
    {
        write<<it3->getId();
        if(it3->getInputs()[0]->getId()>1)
            write<<" "<<it3->getInputs()[0]->getId()+((int)it3->getInputPolarities()[0]);
        else
            write<<" "<<((it3->getInputs()[0]->getId())^((int)it3->getInputPolarities()[0]));
        if(it3->getInputs()[1]->getId()>1)
            write<<" "<<it3->getInputs()[1]->getId()+((int)it3->getInputPolarities()[1])<<endl;
        else
            write<<" "<<((it3->getInputs()[1]->getId())^((int)it3->getInputPolarities()[1]))<<endl;
    }
    write.close();
}

void aigraph::writeAIG(){
    ofstream write;
    unsigned char charzin;
    int M=this->all_inputs.size()+this->all_latches.size()+this->all_ANDS.size();
    write.open((name+".aig").c_str(),ios::binary|ios::out|ios::trunc);
    write<<"aig "<<M<<" "<<this->all_inputs.size()<<" "<<this->all_latches.size()<<" "<<this->all_outputs.size()<<" "<<this->all_ANDS.size()<<endl;
    
    for(vector<output>::iterator it2=this->all_outputs.begin();it2!=this->all_outputs.end();it2++)
        write<<it2->getId()+((int)it2->getInputPolarity())<<endl;

    //deltas
    for(vector<AND>::iterator it3=this->all_ANDS.begin();it3!=this->all_ANDS.end();it3++)
    {
        unsigned int first;
        if(it3->getInputs()[0]->getId()>1)
        {
            if(it3->getId()<it3->getInputs()[0]->getId()){
                cout<<"Id do nodo deve ser maior que do filho "
                << "possivelmente causado por conversão AAG->AIG" << endl;
            }
            encodeToFile(write,(it3->getId())-(it3->getInputs()[0]->getId()+(it3->getInputPolarities()[0])));
            first=(it3->getInputs()[0]->getId()+(it3->getInputPolarities()[0]));
        }
        else
        {
            encodeToFile(write,(it3->getId())-((it3->getInputs()[0]->getId())^(it3->getInputPolarities()[0])));
            first=((it3->getInputs()[0]->getId())^(it3->getInputPolarities()[0]));
        }
        if(it3->getInputs()[1]->getId()>1){
            if((first) < it3->getInputs()[1]->getId()){
                cout<<"Id do nodo deve ser maior que do filho "
                << "possivelmente causado por conversão AAG->AIG" << endl;
            }
            encodeToFile(write,(first)-(it3->getInputs()[1]->getId()+((int)it3->getInputPolarities()[1])));
        }
        else{
            encodeToFile(write,(first)-((it3->getInputs()[1]->getId())^((int)it3->getInputPolarities()[1])));
        }
    }
}

unsigned aigraph::getIndex(unsigned id, unsigned A){
    //entrada
    if(id < 2*(this->all_inputs.size()+1)){
        return (id - 2)/2;
    }
    //and
    else if(id < 2*(this->all_inputs.size()+A+1)){
        return (id - 2*(this->all_inputs.size()+1))/2;
    }
    else{
        cout << "não sei que ID é";
        return 0;
    }
}


nodeAig* aigraph::findAny(unsigned int param, unsigned A){
    unsigned index = 0;
    //constant
    if(param < 2){
        if (param == 0)
        {
            return &this->constant0;
        }
        else{
            return &this->constant1;
        }
    }
    //entrada
    else if(param < 2*(this->all_inputs.size()+1)){
        return &this->all_inputs[getIndex(param, A)];
    }
    //and
    else if(param < 2*(this->all_inputs.size()+this->all_outputs.size()+A+1)){
        return &this->all_ANDS[getIndex(param, A)];
    }
    else{
        cout<<"ERROR: no node found with name "<<param<<" in findAny"<<endl;
        return nullptr;
    }
}

void aigraph::renameNodes(vector<vector<unsigned>> &ands){
    vector<unsigned> nodes, outs;
    unsigned id = 2*(this->all_inputs.size()+1), li = 2*(this->all_inputs.size()+1)-1;
    for(unsigned i=0; i < this->all_outputs.size(); i++){
        if(this->all_outputs[i].getId() > li){
            unsigned index = this->all_outputs[i].getId();
            if(index%2==1){
                index = index - 1;
            }
            nodes.push_back(index);
            outs.push_back(getIndex(index,ands.size()));

        }
    }
    while(!nodes.empty()){
        unsigned node = nodes.back();
        unsigned nodeIndex = getIndex(node,ands.size());
        if(ands[nodeIndex][3]==0){
            unsigned remove = 0;
            unsigned child1 = ands[nodeIndex][1];
            unsigned index1 = child1;
            if(index1%2==1){
                index1 = index1 - 1;
            }
            index1 = getIndex(index1,ands.size());
            if(child1 > li){
                if(ands[index1][3]==0){
                    nodes.push_back(child1);
                }
                else{
                    remove++;
                }
            }
            else{
                remove++;
            }
            unsigned child2 = ands[nodeIndex][2];
            unsigned index2 = child2;
            if(index2%2==1){
                index2 = index2 - 1;
            }
            index2 = getIndex(index2,ands.size());
            if(child2 > li){
                if(ands[index2][3]==0){
                nodes.push_back(child2);
                }
                else{
                    remove++;
                }
            }
            else{
                remove++;
            }
            if(remove==2){
                
                if(ands[nodeIndex][0]%2==1){
                    ands[nodeIndex][0] = id + 1;
                }
                else{
                    ands[nodeIndex][0] = id;
                }
                if(child1 > li){
                    if(ands[nodeIndex][1]%2==1){
                        ands[nodeIndex][1] = ands[getIndex(ands[nodeIndex][1],ands.size())][0] + 1;
                    }
                    else{
                        ands[nodeIndex][1] = ands[getIndex(ands[nodeIndex][1],ands.size())][0] + 1;
                    }
                }
                if(child2 > li){
                    if(ands[nodeIndex][2]%2==1){
                        ands[nodeIndex][2] = ands[getIndex(ands[nodeIndex][2],ands.size())][0] + 1;
                    }
                    else{
                        ands[nodeIndex][2] = ands[getIndex(ands[nodeIndex][2],ands.size())][0] + 1;
                    }
                }
                ands[nodeIndex][3] = 1;
                id = id+2;
                nodes.pop_back();
                if(ands[nodeIndex][1] < ands[nodeIndex][2]){
                    unsigned temp = ands[nodeIndex][1];
                    ands[nodeIndex][1] = ands[nodeIndex][2];
                    ands[nodeIndex][2] = temp;
                }
            }
        }
        else{
            nodes.pop_back();
        }
    }
    unsigned tIndex = 0;
    for(unsigned i=0; i < all_outputs.size(); i++){
        if(this->all_outputs[i].getId() > li)
        {
            unsigned ind = outs[tIndex++];
            if(this->all_outputs[i].getId()%2==1){
                this->all_outputs[i].setId(ands[ind][0]+1);
            }
            else{
                this->all_outputs[i].setId(ands[ind][0]);
            }
            
        }
    }  
}

void aigraph::readAAG(ifstream& file, string name){
    
    string line;
    string type;
    unsigned int M,I,L,O,A;
    unsigned int i,lhs,rhs0,rhs1;
    
    if(name.find("/"))
        name.erase(0,name.find_last_of('/')+1);
    if(name.find("."))
        name.erase(name.find_last_of('.'),name.size());
    file.seekg(file.beg);
    getline(file,line);
    type=wordSelector(line,1);
    M=stoi(wordSelector(line,2));
    I=stoi(wordSelector(line,3));
    L=stoi(wordSelector(line,4));
    O=stoi(wordSelector(line,5));
    A=stoi(wordSelector(line,6));

    if(L!=0){
        cout << "Latches não suportados";
        return;
    }
    this->setName(name);


    bool polarity0,polarity1,lhs_polarity;

    vector<input> in(I);
    this->all_inputs = in;

    //////////////////////INPUTS///////////////////
    for(i=0;i<I;i++){
        getline(file,line);
        this->all_inputs[i].setId(stoi(line));
    }

    //////////////////OUTPUTS///////////////////
    
    vector<output> out(O);
    this->all_outputs = out;

    for(i=0;i<O;i++){
        getline(file,line);
        lhs=stoi(wordSelector(line,1));
        this->all_outputs[i].setId(lhs);
    }

   ///////////////////ANDS////////////////
    vector<AND> a(A);
    this->all_ANDS = a;
    unsigned and_index = -1;
    vector<vector<unsigned>> ands(A);
    for(i=0;i<A;i++){
        getline(file,line);
        vector<unsigned> vand(4);
        vand[0] = stoi(wordSelector(line,1));
        vand[1] = stoi(wordSelector(line,2));
        vand[2] = stoi(wordSelector(line,3));
        ands[getIndex(vand[0],A)] = vand;
    }

    renameNodes(ands);

    for(i=0;i<A;i++){
        getline(file,line);
        lhs=ands[i][0];
        this->all_ANDS[getIndex(lhs, A)].setId(lhs);
        //solving inputs' polarity
        rhs0=ands[i][1];
        if(rhs0%2!=0)
        {
            rhs0--;
            polarity0=true;
        }
        else{
            polarity0=false;
        }
        
        rhs1=ands[i][2];
        if(rhs1%2!=0)
        {
            rhs1--;
            polarity1=true;
        }
        else{
            polarity1=false;
        }

        //solving the AND's inputs
        //rhs0
        if(this->findAny(rhs0, A)!=nullptr) //if input already exists
        {
            //add the AND to the output list of the rhs0 outputs
            this->findAny(rhs0, A)->pushOutput(this->findAny(lhs, A));
            this->findAny(lhs, A)->pushInput(this->findAny(rhs0, A),polarity0);
        }
        else{
            cout << "Eu achava que as entradas tinham que ser declaradas antes" << endl;
        }

        //the same for the second input
        //rhs1
        if(this->findAny(rhs1, A)!=nullptr) //if input already exists
        {
            //add the AND to the output list of the rhs1 outputs
            this->findAny(rhs1, A)->pushOutput(this->findAny(lhs, A));
            this->findAny(lhs, A)->pushInput(this->findAny(rhs1, A),polarity1);
        }
        else{
            cout << "Eu achava que as entradas tinham que ser declaradas antes" << endl;
        }

    }
    file.close();

    
    vector<output>::iterator it_out;
    for(it_out=this->all_outputs.begin();it_out!=this->all_outputs.end();it_out++)
    {   
        if(it_out->getId() >=2){
            if(it_out->getId() % 2 != 0)
            {
                it_out->setId(it_out->getId()-1);
                polarity0=true;
            }
            else{
                polarity0=false;
            }
        }
        else{
            polarity0=false;
        }
        
        it_out->pushInput(findAny(it_out->getId(), A),polarity0);
    }
}

void aigraph::encodeToFile(ofstream& file, unsigned x){
        unsigned char ch;
        while (x & ~0x7f)
        {
            ch = (x & 0x7f) | 0x80;
            file<<ch;
            x >>= 7;
        }
        ch = x;
        file<<ch;
}

unsigned char aigraph::getnoneofch (ifstream& file,int index){
        
        int ch;
        char c;
        file.get(c);
        ch=(int)c;
        if (ch != '\0')
            return ch;
        else
        {
            cout<<"AND:"<<index<<" ch:"<<ch<<" c:"<<c<<endl;
            fprintf (stderr, "*** decode: unexpected EOF\n\n");
            return ch;
        }
    }

unsigned aigraph::decode (ifstream& file,int index){
        unsigned x = 0, i = 0;
        unsigned char ch;
        while ((ch = getnoneofch (file,index)) & 0x80)
        {
            x |= (ch & 0x7f) << (7 * i++);
        }
        return x | (ch << (7 * i));
    }

void aigraph::readAIG(ifstream& file, string param_name){
    string line;
    string type;
    unsigned int M,I,L,O,A;
    unsigned int lhs,rhs0,rhs1, delta1,delta2;
    if(param_name.find("/"))
        param_name.erase(0,param_name.find_last_of('/')+1);
    if(param_name.find("."))
        param_name.erase(param_name.find_last_of('.'),param_name.size());
    this->setName(param_name);
    file.seekg(file.beg);
    getline(file,line);
    //line has the index information
    type=wordSelector(line,1);
    M=stoi(wordSelector(line,2));
    I=stoi(wordSelector(line,3));
    L=stoi(wordSelector(line,4));
    O=stoi(wordSelector(line,5));
    A=stoi(wordSelector(line,6));

    vector<input> in(I);
    this->all_inputs = in;
    vector<AND> a(A);
    this->all_ANDS = a;

    bool polarity0,polarity1,lhs_polarity;
    
    for(int f=0;f<O;f++){
        getline(file,line);
    }

    //////////////////////INPUTS//////////////////////
    for(int i=1;i<=I;i++)
    {
        this->all_inputs[i-1].setId(i*2);
    }
    

    /////////////ANDS/////////////////////////
    int and_index=I+L;
    bool polar=false;
    AND* AND_ptr;
    for(int l=0;l<A;l++)
    {
        and_index++;
        this->all_ANDS[l].setId(and_index*2);
        AND_ptr = &this->all_ANDS[l];
        
        delta1=decode(file,and_index*2);
        rhs0=and_index*2-delta1;
        delta2=decode(file,and_index*2);
        rhs1=rhs0-delta2;
        
        if(rhs0 % 2!=0)
        {
            polar=true;
            rhs0--;
        }
        else{
            polar=false;
        }
        AND_ptr->pushInput(this->findAny(rhs0, A),polar);
        if(rhs1 % 2!=0)
        {
            polar=true;
            rhs1--;
        }
        else{
            polar=false;
        }
        AND_ptr->pushInput(this->findAny(rhs1, A),polar);
        
#if IGNORE_OUTPUTS == 0
    if(rhs0>1)
       this->findAny(rhs0, A)->pushOutput(AND_ptr);
    if(rhs1>1)
       this->findAny(rhs1, A)->pushOutput(AND_ptr); 
#endif
    }

        ////////////////////OUTPUTS///////////////////
    file.seekg(file.beg);
    getline(file,line);

    vector<output> out(O);
    this->all_outputs = out;
    
    for(int f=0;f<O;f++){
        getline(file,line);
       lhs=stoi(wordSelector(line,1));
       if(lhs>1)
       {
            if(lhs % 2 != 0)
            {
                lhs=lhs-1;
                polarity0=true;
            }
            else
                polarity0=false;
       }
       else{
           polarity0=false;
       }
       this->all_outputs[f].setId(lhs);
        this->all_outputs[f].pushInput(findAny(lhs, A),polarity0);
    }
}


void aigraph::printCircuit() {
    cout<<"Circuit name: "<<this->name<<endl;
    cout<<"MILOA:"<<this->all_ANDS.size()+this->all_inputs.size()+this->all_latches.size()<<","<<this->all_inputs.size()<<","<<this->all_latches.size()<<","<<this->all_outputs.size()<<","<<this->all_ANDS.size()<<endl;
    vector<AND>::iterator it_and;
    vector<input>::iterator it_in;
    vector<output>::iterator it_out;
    vector<latch>::iterator it_latch;
    
    cout<<"Inputs: ";
    for(it_in=this->all_inputs.begin();it_in!=this->all_inputs.end();it_in++){
        cout<<it_in->getId()<<",";
    }
    cout<<endl;
    
    cout<<"Latches: ";
    for(it_latch=this->all_latches.begin();it_latch!=this->all_latches.end();it_latch++){
        cout<<it_latch->getId()<<",";
    }
    cout<<endl;
    
    cout<<"Outputs: ";
    for(it_out=this->all_outputs.begin();it_out!=this->all_outputs.end();it_out++){
        cout<<it_out->getId()+((int)it_out->getInputPolarity())<<",";
    }
    cout<<endl;
    
    cout<<"ANDs: ";
    for(it_and=this->all_ANDS.begin();it_and!=this->all_ANDS.end();it_and++)
    {
        if(it_and->getInputs().size()==2){
            cout<<it_and->getId()<<"("<<it_and->getInputs()[0]->getId()+((int)it_and->getInputPolarities()[0])<<","<<it_and->getInputs()[1]->getId()+((int)it_and->getInputPolarities()[1])<<"),";
        }
 
        else
        {
            cout<<"ERROR, unexpected size of AND "<<it_and->getId()<<":"<<it_and->getInputs().size()<<endl;
            for(int a=0;a<it_and->getInputs().size();a++)
                cout<<"intput "<<a+1<<":"<<it_and->getInputs()[a]->getId()<<endl;
        }
    }
    cout<<endl;
}



void aigraph::setDepthsInToOut(){
    cout<<"Setting AIG depths, NODE TO -->PI<--"<<endl;
    int retval,depth=0;
    vector<AND>::iterator it_and;
    vector<input>::iterator it_in;
    vector<output>::iterator it_out;
    ofstream write;
    
    //Initializing depths
    for(it_in=this->all_inputs.begin();it_in!=this->all_inputs.end();it_in++)
        it_in->setDepth(-1);
    for(it_and=this->all_ANDS.begin();it_and!=this->all_ANDS.end();it_and++)
        it_and->setDepth(-1);
    for(it_out=this->all_outputs.begin();it_out!=this->all_outputs.end();it_out++)
        it_out->setDepth(-1);
    for(it_out=this->all_outputs.begin();it_out!=this->all_outputs.end();it_out++)
    {
        depth=it_out->computeDepthInToOut();
        it_out->setDepth(depth);
    }
    int greater=-1;
    for(it_out=this->all_outputs.begin();it_out!=this->all_outputs.end();it_out++)
    {
        if(it_out->getDepth()>greater)
        {
            greater=it_out->getDepth();
            greatest_depths_ids.clear();
            greatest_depths_ids.push_back(it_out->getId());
        }
        else if (it_out->getDepth()==greater)
            greatest_depths_ids.push_back(it_out->getId());
    }
    this->graph_depth=greater;
    cout<<"setting depths done."<<endl;
}

void aigraph::setShortestDistanceToPO(){
    cout<<"Setting AIG depths, -->PO<--"<<endl;
    int depth=0;
    vector<AND>::iterator it_and;
    vector<input>::iterator it_in;
    vector<output>::iterator it_out;
    ofstream write;
    //Initializing depths
    for(it_in=this->all_inputs.begin();it_in!=this->all_inputs.end();it_in++)
        it_in->setDepth(-1);
    for(it_and=this->all_ANDS.begin();it_and!=this->all_ANDS.end();it_and++)
        it_and->setDepth(-1);
    for(it_out=this->all_outputs.begin();it_out!=this->all_outputs.end();it_out++)
        it_out->setDepth(-1);
    
    
    
    for(it_and=this->all_ANDS.begin();it_and!=this->all_ANDS.end();it_and++)
        cout<<it_and->getId()<<":"<<it_and->getDepth()<<endl;
    int greater=-1;
    for(it_in=this->all_inputs.begin();it_in!=this->all_inputs.end();it_in++)
    {
        cout<<it_in->getId()<<":"<<it_in->getDepth()<<endl;
        if(it_in->getDepth()>greater)
        {
            greater=it_in->getDepth();
            greatest_depths_ids.clear();
            greatest_depths_ids.push_back(it_in->getId());
        }
        else if (it_in->getDepth()==greater)
            greatest_depths_ids.push_back(it_in->getId());
    }
    this->graph_depth=greater;
    cout<<"graph depth:"<<this->graph_depth<<endl;
    
    write.open("Depths.txt");
    write<<this->name<<","<<greater<<endl;
    for(it_and=this->all_ANDS.begin();it_and!=this->all_ANDS.end();it_and++)
        write<<it_and->getId()<<":"<<it_and->getDepth()<<endl;
    write.close();
}

void aigraph::printDepths(){
    vector<AND>::iterator it;
    vector<input>::iterator it2;
    vector<output>::iterator it3;
    
    cout<<"Inputs' depths:"<<endl;
    for(it2=this->all_inputs.begin();it2!=this->all_inputs.end();it2++)
        cout<<it2->getId()<<":"<<it2->getDepth()<<endl;
    cout<<endl;
    
    cout<<"Outputs' depths:"<<endl;
    for(it3=this->all_outputs.begin();it3!=this->all_outputs.end();it3++)
        cout<<it3->getId()<<":"<<it3->getDepth()<<endl;
    cout<<endl;
                
    cout<<"ANDs' depths:"<<endl;
    for(it=this->all_ANDS.begin();it!=this->all_ANDS.end();it++)
        cout<<it->getId()<<":"<<it->getDepth()<<"|";//<<endl;
    cout<<endl;
}

/*
--------------------------------------------------------------------
Entrada: Conjunto de valores para cada entrada.
Função: Insere na entradas entradas do AIG os valores referentes a
cada entrada.
Saída:
*/
void aigraph::setInputValue(vector<unsigned long long> vectors){
    unsigned index = 0;
    vector<input>::iterator it;
    if(this->all_inputs.size() != vectors.size()){
        cout << "Tamanho de entrada e vectors diferentes " << this->all_inputs.size() <<
                " " << vectors.size() << " em aigraph::setInputValue";
    }
    for(it=this->all_inputs.begin();it!=this->all_inputs.end();it++)
    {
        it->setBitVector(vectors[index]);
        index++;

    }
}

/*
--------------------------------------------------------------------
Entrada:
Função: Inicializa os atributos signal de cada nodo. Signals são
utilizados na propagação das entradas pelo AIG.
Saída: 
*/
void aigraph::initializeSignals(){
    vector<input>::iterator it_in;
    vector<output>::iterator it_out;
    vector<AND>::iterator it_and;
    for(it_in=this->all_inputs.begin();it_in!=this->all_inputs.end();it_in++)
        it_in->setSignal(2);
    for(it_and=this->all_ANDS.begin();it_and!=this->all_ANDS.end();it_and++)
        it_and->setSignal(-1);
    for(it_out=this->all_outputs.begin();it_out!=this->all_outputs.end();it_out++)
        it_out->setSignal(-1);
}

/*
--------------------------------------------------------------------
Entrada: vSize->Número de vetores utilizados nesta simulação.
Função: Obtem o valor em magnitude das saídas do AIG, considerando 
que vSize vetores foram utilizados na entrada.
Saída: vSize Valores de saída do AIG.
*/
vector<unsigned long long> aigraph::calcOutputValue(unsigned vSize){
    unsigned long long one = 1;
    unsigned outSize = this->all_outputs.size();
    vector<unsigned long long> ret(vSize,0);
    for(unsigned outIndex = 0; outIndex < outSize; outIndex++){
        unsigned long long out = this->all_outputs[outIndex].getBitVector();
        for(unsigned i = 0; i<vSize;i++){
            //ret[vSize-i-1] = ret[vSize-i-1]|(((out>>i)&one)<<(outSize-outIndex-1));
            ret[vSize-i-1] = ret[vSize-i-1]|(((out>>i)&one)<<(outIndex));
        }
    }
    return ret;
}

/*
--------------------------------------------------------------------
Entrada: sim_vectors-> Conjunto de vetores de entrada para simulação.
         nVectors-> Numero de vetores de entrada
Função: Para cada conjunto vetores de simulação, coloca nas entradas 
do AIG, propaga os valores pelo AIG e calcula os valores de saída.
Saída: Conjunto de valores de saída.
*/
vector<vector <unsigned long long>> aigraph::simulate(vector<vector<unsigned long long>> sim_vectors, unsigned nVectors){
    vector<vector <unsigned long long>> ret;
    initializeSignals(); 
    unsigned vCount = 0;
    for(vector<unsigned long long> v : sim_vectors){
        /*setInputValue(v);
        vector<output>::iterator it_out;
        for(it_out=this->all_outputs.begin();it_out!=this->all_outputs.end();it_out++){
            it_out->PropagSignalDFS();
        }*/
        propagSignal(v);
        vector<unsigned long long> outputValues = calcOutputValue(((nVectors-vCount)<64) ? (nVectors-vCount) : 64);
        ret.push_back(outputValues);
        vCount =+64;
        initializeSignals(); 
    }
    return ret;
}

void aigraph::propagSignal(vector<unsigned long long> vectors){
    //inputs não inicializados em outra função
    //poderia ser junto
    unsigned long long int sig_rhs0,sig_rhs1;

    unsigned index = 0;
    vector<input>::iterator it;
    if(this->all_inputs.size() != vectors.size()){
        cout << "Tamanho de entrada e vectors diferentes " << this->all_inputs.size() <<
                " " << vectors.size() << " em aigraph::propagSignal";
    }
    for(it=this->all_inputs.begin();it!=this->all_inputs.end();it++)
    {
        it->setBitVector(vectors[index]);
        index++;

    }

    for (int i = 0; i < all_ANDS.size(); i++){
        sig_rhs0=all_ANDS[i].getInputs()[0]->getBitVector();
        sig_rhs1=all_ANDS[i].getInputs()[1]->getBitVector();
        if(all_ANDS[i].getInputPolarities()[0]){
            sig_rhs0=~sig_rhs0;
        }
        if(all_ANDS[i].getInputPolarities()[1]){
            sig_rhs1=~sig_rhs1;
        }
        all_ANDS[i].setBitVector(sig_rhs0 & sig_rhs1);
    }

    for (int i = 0; i < all_outputs.size(); i++){
        sig_rhs0=all_outputs[i].getInput()->getBitVector();
        if(all_outputs[i].getInputPolarity()){
            sig_rhs0=~sig_rhs0;
        }
        all_outputs[i].setBitVector(sig_rhs0);
    }
}

void aigraph::printAllNodesBitVector(int nVectors){
    for(input i : all_inputs){
        cout << "i" << i.getId() << " ";
        printBit(i.getBitVector(),nVectors);
        cout << endl;
    }
    for(AND a : all_ANDS){
        cout << "a" << a.getId() << " "; 
        printBit(a.getBitVector(),nVectors);
        cout << endl;
    }
    for(output o : all_outputs){
        cout << "o" << o.getId() << " "; 
        printBit(o.getBitVector(),nVectors);
        cout << endl;
    }
}

void aigraph::printBit(unsigned long long vector, int size){
    unsigned j = 0, m=0;
    unsigned long long mask = 1, value = 1;
    for(int i = size-1; i >=0; i--){
        mask = mask << i;
        value = vector & mask;
        if(value > 0){
            cout <<  "1";
        }
        else{
            cout <<  "0";
        }
        j++;
        if(j==16){
            j=0;
            cout << endl;
        }
        mask = 1;
    }
}