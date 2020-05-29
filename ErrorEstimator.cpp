#include "ErrorEstimator.h"

using namespace std;

/*
--------------------------------------------------------------------
Entrada: Orig->Uma instância da classe
Função: Construtor da classe.
Saída:
*/
ErrorEstimator::ErrorEstimator(const ErrorEstimator& orig) {
}

/*
--------------------------------------------------------------------
Entrada: graph_orig->AIG referência; 
		 nVectors->Quantidade de vetores de teste
Função: Construtor da classe.
Saída:
*/
ErrorEstimator::ErrorEstimator(aigraph &graph_obj, unsigned nVectors) {
	//this->graph_orig = graph_obj;
	//Corrigir o valor de nVectors
	this->nInput = graph_obj.getInputs()->size();
    unsigned long long one = 1;
	if(nVectors > (one<<this->nInput) && this->nInput<64){
        this->nVectors = (one<<this->nInput);
        //cout << "nVectors atualizado para " << this->nVectors << endl;
    }
    else{
    	this->nVectors = nVectors;
    }
	generateSimVectors();
	this->out_orig = graph_obj.simulate(this->sim_vectors, this->nVectors);
	vector<double> e(7,0.0);
	this->errors = e;
    this->nOutput = graph_obj.getOutputs()->size();
}

/*
--------------------------------------------------------------------
Entrada:
Função: Destrutor da classe.
Saída: 
*/
ErrorEstimator::~ErrorEstimator() {
}

/*
--------------------------------------------------------------------
Entrada:
Função: Limpa atributos do objeto.
Saída: 
*/
void ErrorEstimator::clearErrorEstimator(){
	//this->graph_orig.clearCircuit();
    this->sim_vectors.clear();
    this->out_orig.clear();
    this->errors.clear();
}

/*
--------------------------------------------------------------------
Entrada:
Função: Retorna vetores para simulação.
Saída: Vetores para simulação.
*/
vector<vector<unsigned long long>> ErrorEstimator::getSimVectors(){
	return this->sim_vectors;
}

/*
--------------------------------------------------------------------
Entrada:
Função: Retorna valores de saída do grafo original, considerando todos
os vetores de simulação.
Saída: Valores de saída do grafo original.
*/
vector<vector<unsigned long long>> ErrorEstimator::getOutOrig(){
	return this->out_orig;
}

/*
--------------------------------------------------------------------
Entrada:
Função: Retorna o valor referente ao Worst Bit Flip.
Saída: Valor referente ao Worst Bit Flip.
*/
unsigned ErrorEstimator::getWBF(){
	return round(this->errors[0]);
}

/*
--------------------------------------------------------------------
Entrada:
Função: Retorna o valor referente ao Error Rate.
Saída: Valor referente ao Error Rate.
*/
double ErrorEstimator::getER(){
	return this->errors[1];
}

/*
--------------------------------------------------------------------
Entrada:
Função: Retorna o valor referente ao Mean Absolute Error.
Saída: Valor referente ao Mean Absolute Error.
*/
double ErrorEstimator::getMAE(){
	return this->errors[2];
}

/*
--------------------------------------------------------------------
Entrada:
Função: Retorna o valor referente ao Mean Squared Error.
Saída: Valor referente ao Mean Squared Error.
*/
double ErrorEstimator::getMSE(){
	return this->errors[3];
}

/*
--------------------------------------------------------------------
Entrada:
Função: Retorna o valor referente ao Mean Relative Error.
Saída: Valor referente ao Mean Relative Error.
*/
double ErrorEstimator::getMRE(){
	return this->errors[4];
}

/*
--------------------------------------------------------------------
Entrada:
Função: Retorna o valor referente ao Worst Case Error.
Saída: Valor referente ao Worst Case Error.
*/
unsigned long long ErrorEstimator::getWCE(){
	return llround(this->errors[5]);
}

/*
--------------------------------------------------------------------
Entrada:
Função: Retorna o valor referente ao Worst Case Relative Error.
Saída: Valor referente ao Worst Case Relative Error.
*/
double ErrorEstimator::getWCRE(){
	return this->errors[6];
}

/*
--------------------------------------------------------------------
Entrada:
Função: Gera aleatóriamente os vetores para simulação e coloca no 
atributo sim_vectors.
Saída: 
*/
void ErrorEstimator::generateSimVectors(){
	unordered_set<unsigned long long> vectors;
    vector<vector<unsigned long long>> ret;
    //unsigned nInput = this->graph_orig.getInputs()->size();
    unsigned long long div = 1;
    if(this->nInput<64){
        div = div<<this->nInput;
    }
    else if(this->nInput==64){
        div = ULLONG_MAX;
    }
    
    if(this->nInput<=64){
        if(this->nVectors < div){
            mt19937_64 eng(time(0));
            for(unsigned i = 0; i < this->nVectors; i++){  
                vectors.insert(eng()%div);
            }
            while(vectors.size() < this->nVectors){
                vectors.insert(eng()%div);
            }
        }
        else{
            for(unsigned long long i = 0; i < div; i++){  
                vectors.insert(i);
            }
        }

        unsigned long long value = 1;
        unsigned i = 0;
        vector<unsigned long long> bash(this->nInput,0);
        for(unsigned long long v : vectors){
            for(int m = 0; m<this->nInput; m++){
                if((v & (value<<m))>0){
                    bash[m] = bash[m] | (value << i);
                }
            }
            i++;
            if(i == 64){
                ret.push_back(bash);
                for(int b = 0; b < this->nInput; b++){
                    bash[b] = 0;
                }
                i=0;
            }
        }
        if(i>0){
            ret.push_back(bash);
        }
    }
    else{//> 64bits de entrada
        unsigned i = 0;
        vector<unsigned long long> bash;
        mt19937_64 eng(time(0));
        for(unsigned i = 0; i < this->nVectors; i++){  
            bash.push_back(eng());
            i++;
            if(i == 64){
                ret.push_back(bash);
                bash.clear();
                i=0;
            }
        }
        if(i>0){
            ret.push_back(bash);
        }
    }
    this->sim_vectors = ret;
}

/*
--------------------------------------------------------------------
Entrada:
Função: Zera todas as posições do errors.
Saída: 
*/
void ErrorEstimator::clearError(){
	for(int i = 0; i < errors.size(); i++){
		this->errors[i] = 0;
	}
}

/*
--------------------------------------------------------------------
Entrada:graph_app-> AIG aproximado.
Função: Obtem os valores de saída do grafo aproximado e chama o 
método para calcular o erro
Saída: 
*/
void ErrorEstimator::computeError(aigraph &graph_app){
    if(graph_app.getInputs()->size() != this->nInput || graph_app.getOutputs()->size() != this->nOutput){
        cout << "Número de entradas e saídas dos AIGs devem ser iguais." << endl;
        return;
    }
	vector<vector <unsigned long long>> out_app = graph_app.simulate(this->sim_vectors, this->nVectors);
	computeError(out_app);
}

/*
--------------------------------------------------------------------
Entrada:out_app-> Vetor dos valores de saída aproximado.
Função: Calcula os erros entre os valores do out_orig e do out_app.
Os erros calculados são WBF, ER, MAE, MSE, MRE, WCE, WCRE, em ordem.
Saída: 
*/
void ErrorEstimator::computeError(vector<vector <unsigned long long>> out_app){
	clearError();
	unsigned long long bitDiff = 0, one = 1;
    double valueDiff = 0.0;
    unsigned count = 0;
    //unsigned nInput = this->graph_orig.getInputs()->size();
    for(unsigned i = 0;i<this->out_orig.size();i++){
    	for(unsigned j = 0;j<this->out_orig[i].size();j++){
	    	bitDiff = this->out_orig[i][j] ^ out_app[i][j];
            long long unsigned ll = llabs(out_app[i][j] - this->out_orig[i][j]);
	        valueDiff = (double)ll;
	        for (int j = 0; j < this->nOutput; j++)
	        {
	            count = count + ((bitDiff>>j)&one);
	        }
	        if((double)count>errors[0]){
	            errors[0]=(double)count;
	        }
	        if(count>0){
	            errors[1]=errors[1]+1.0;
	        }
	        errors[2]= errors[2]+valueDiff;
	        errors[3]= errors[3]+pow(valueDiff,2);
	        double relative;
	        if(this->out_orig[i][j] == 0){
	            relative = valueDiff;
	        }
	        else{
	            relative = valueDiff/(double)this->out_orig[i][j];
	        }
            /*if(relative > 1.0){
                cout << this->sim_vectors[i][j] << " ";
                cout << out_app[i][j] << " ";
                cout << out_orig[i][j] << " ";
                cout <<  ll << " ";
                cout << valueDiff << " ";
                cout << relative << endl;

            }*/

	        errors[4]=errors[4]+relative;
	        if(valueDiff>errors[5]){
	            errors[5]=valueDiff;
	        }
	        if(relative>errors[6]){
	            errors[6]=relative;
	        }
	        count = 0;
    	}
    }
    errors[1]=errors[1]/(double)this->nVectors;
    errors[2]=errors[2]/(double)this->nVectors;
    errors[3]=errors[3]/(double)this->nVectors;
    //cout << errors[4] << " " << this->nVectors << " ";
    errors[4]=errors[4]/(double)this->nVectors;
    //cout << errors[4] << endl;
}
