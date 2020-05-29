#include "AIG.h"
#include "ErrorEstimator.h"
using namespace std;

vector<nodeAig> vec;

void abcWrite(string new_name,string abc_name){
    cout<<endl<<"ABC WRITE: ("<<new_name<<") -> ("<<abc_name<<")"<<endl;
    ofstream script("script.scr");
    script<<"&r "<<new_name<<".aig"<<endl<<"&ps"<<endl<<"&w "<<abc_name<<endl<<"quit";
    script.close();
    system("./../abc -c 'source script.scr' >> log.txt ");
}

void abcCeC(string new_name,string abc_name,float min_th,int option){
    cout<<endl<<"ABC CEC: ("<<new_name<<") VS ("<<abc_name<<")"<<endl;
    ofstream script("script.scr"),log("log.txt",ios::app);
    script<<"&cec "<<new_name<<".aig "<<abc_name<<endl<<"quit";
    script.close();
    log<<"CEC on circuits: "<<new_name<<" VS: "<<abc_name<<endl<<"TH:"<<min_th<<", OPTION:"<<option<<endl;
    log.close();
    system("./../abc -c 'source script.scr' >> log.txt");
}

/*void printUsedVector(vector<unsigned long long > v, int nVectors){
    for(unsigned long long vv : v){
            printBit(vv,nVectors);
            cout << endl;
        }
}
*/
void printError(vector<double> errors){
    cout << "WBF " << errors[0] << endl;
    cout << "ER " << errors[1] << endl;
    cout << "MAE "<< errors[2] << endl;
    cout << "MSE "<< errors[3] << endl;
    cout << "MRE "<< errors[4] << endl;
    cout << "WCE "<< errors[5] << endl;
    cout << "WCRE "<< errors[6] << endl;
}


int main(int argc, char** argv) {
    string file_orig = "./ha.aag", file_app;
    int nVectors = 1000;
    ifstream read_orig, read_app;
    aigraph graph_orig, graph_app;
    if(argc > 1){
        nVectors = atoi(argv[1]);
    }
    if(argc > 2){
        file_orig = "./";
        file_orig = file_orig + argv[2];
        read_orig.open(file_orig.c_str(),ifstream::binary);
        
        if(file_orig.compare(file_orig.length()-3,3,"aag")==0){
            graph_orig.readAAG(read_orig,file_orig);
        }
        else if(file_orig.compare(file_orig.length()-3,3,"aig")==0){
            graph_orig.readAIG(read_orig,file_orig);
        }
        else{
            cout << "Formato desconhecido. Deve ser aig ou aag." << endl;
        }
        if(argc == 3){
            cout << "Comando deve ser: ./main [nVectors] [aig/aag file] [aig/aag file]" << endl;
            return 0;
        }
        file_app = "./";
        file_app = file_app + argv[3];
        read_app.open(file_app.c_str(),ifstream::binary);
        
        if(file_app.compare(file_app.length()-3,3,"aag")==0){
            graph_app.readAAG(read_app,file_app);
        }
        else if(file_app.compare(file_app.length()-3,3,"aig")==0){
            graph_app.readAIG(read_app,file_app);
        }
        else{
            cout << "Formato desconhecido. Deve ser aig ou aag." << endl;
        }
    }
    else{
        cout << "Lendo arquivo padrão " << file_orig << endl;
        read_orig.open(file_orig.c_str(),ifstream::binary);
        graph_orig.readAAG(read_orig,file_orig);
    }
    //graph_orig.printCircuit();
    //graph_app.printCircuit();
    ErrorEstimator estimator(graph_orig, nVectors);
    estimator.computeError(graph_app);
    cout << estimator.getWBF();
    cout << "\t" << estimator.getER();
    cout << "\t" << estimator.getMAE();
    cout << "\t" << estimator.getMSE();
    cout << "\t" << estimator.getMRE();
    cout << "\t" << estimator.getWCE();
    cout << "\t" << estimator.getWCRE() << endl;
}
int main_teste(int argc, char** argv) {
    string file_orig = "./ha_t.aag";
    ifstream read_orig;
    aigraph graph_orig;
    read_orig.open(file_orig.c_str(),ifstream::binary);
    graph_orig.readAAG(read_orig,file_orig);
    graph_orig.writeAIG();
}