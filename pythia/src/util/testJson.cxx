#include <iostream>
#include <json.h>
#include <fstream>

using namespace std;
void displayCfg(const Json::Value& cfg);

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

	for(int index = 0; index < pT.size(); ++index){
		double jet_pt = pT[index].asDouble();
		// string jet_pt = pT[index].asString();
		cout << jet_pt <<" " << endl;
	}


	return 0;
}

void displayCfg(const Json::Value& cfg){
	return;
}
