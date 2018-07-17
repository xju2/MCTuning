#include <iostream>
#include <json.h>
#include <fstream>

using namespace std;
void displayCfg(const Json::Value& cfg);
void buildStaticVariable(const Json::Value& Resolution);
vector<double> jet_pT_bins;
/****
class Node{
	double pT;
	double eta;
	double val;
	Node* next_pT;
	Node* next_eta;
public:
	Node(double pT, double eta, double val, Node* next_pT, Node* next_eta=NULL){
		this->pT = pT;
		this->eta = eta;
		this->val = val;
		this->next_pT = next_pT;
		this->next_eta = next_eta;
	}
	Node(){
		pT = eta = val = 0.;
		next_pT = next_eta = NULL;
	}
	double get() {return val;}
	Node* next_pt() { return this->next_pT; }
	Node* next_eta() { return this->next_eta; }
}
};
class DetectorCorrection
	vector<double> pT_bins;
	vector< vector<double> >  eta_pT_bins;
	vector< vector<double> > values;
	Node* root;
public:
	DetectorCorrection(const Json::Value& json_input);
	void clear(){
		pT_bins.clear();
		eta_bins.clear();
	}
	double get(double pT, double eta);
};

DetectorCorrection::DetectorCorrection(const Json::Value& json_input){
	this->clear();
	const Json::Value& pT = json_input["pT"];
	if(pT.type() == Json::nullValue){
		cout << "ERROR, DetectorCorrection is not constructed well" << endl;
	}
	for(int index = 0; index < pT.size(); ++ index){
		pT_bins.push_back(pT[index].asDouble());
	}

	const Json::Value& pT_values = json_input["values"];

	bool has_eta_dep = pT_values[0].type() != Json::realValue;
	if(!has_eta_dep){
		eta_bins.push_back(-5);
		eta_bins.push_back(5);
	}
	
	for(int index = 0; index < pT_values.size(); ++ index){
		if(has_eta_dep){
			const Json::Value& eta_input = pT_values[index]["eta"];
			vector<double> tmp_eta;
			for(int eta_id = 0; eta_id < eta_input.size(); ++eta_id){
				tmp_eta.push_back(eta_input[eta_id].asDouble());
			}
		}

	}
}
**/

int main(int argc, char** argv) {
	if (argc < 2){
		cout << argv[0] << " json" << endl;
		return 1;
	}

	Json::Value value;
	ifstream cfgfile(argv[1], ifstream::binary);
	cfgfile >> value;

	cout << "---- start ----" << endl;
	cout << value << endl;
	cout << "---- end ----" << endl;

	cout << value.size() << endl;
	const Json::Value jets = value["Jets"];
	cout <<"Jets: " << jets.size() << ", type: " << jets.type() << endl;
	const Json::Value resolution = jets["Run1_Resolution"];
	cout <<"Jets, Resolution: " << resolution.size() << ", type: " << resolution.type() << endl;
	const Json::Value pT = resolution["pT"];
	cout <<"Jets, pT: " << pT.size() << ", type: " << pT.type() << endl;
	const Json::Value eta = resolution["eta"];
	if (eta.type() == Json::nullValue){
		cout << "no eta range, then use [-5, 5]" << endl;
	}
/**
	for(int index = 0; index < pT.size(); ++index){
		double jet_pt = pT[index].asDouble();
		// string jet_pt = pT[index].asString();
		cout << jet_pt <<" " << endl;
	}
***/
	buildStaticVariable(resolution);
	for(auto pT_bin: jet_pT_bins){
		cout << pT_bin <<" " << endl;
	}



	return 0;
}

void displayCfg(const Json::Value& cfg){
	return;
}

void buildStaticVariable(const Json::Value& resolution)
{
	const Json::Value pT = resolution["pT"];
	for(int index = 0; index < pT.size(); ++ index){
		jet_pT_bins.push_back(pT[index].asDouble());
	}
}
