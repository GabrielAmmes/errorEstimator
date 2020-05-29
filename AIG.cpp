/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "AIG.h"

string wordSelector(string line, int word_index){
    int i=1;
    line.erase(0,line.find_first_not_of(" "));
    while(i<word_index)
    {
        line.erase(0,line.find_first_not_of(' '));
        line.erase(0,line.find_first_of(' ')+1);
        line.erase(0,line.find_first_not_of(' '));
        i++;
    }
    if(line.find(' ')!=string::npos)
        line.erase(line.find_first_of(' '),line.size());    
    return line;
}



bool getThisPtrPolarity(nodeAig* param){
    nodeAig* aux;
    
    aux=param->fixLSB();
    //returns TRUE for inverted and FALSE for regular
    return ((int)((uintptr_t)param) & 01);
}


int binToDec(vector<int> param){
    int N=param.size();
    int result=0,aux=0;
    result=-param[N-1]*pow(2,N-1);
    
    for(int i=0;i<=N-2;i++)
        aux+=param[i]*pow(2,i);
    result=result+aux;
    return result;
}