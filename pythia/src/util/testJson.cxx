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

	Json::Reader reader;
	Json::Value value;
	ifstream cfgfile(argv[1]);
	cfgfile >> value;

	cout << "---- start ----" << endl;
	cout << value << endl;
	cout << "---- end ----" << endl;

	return 0;
}

void displayCfg(const Json::Value& cfg){
	return;
}
