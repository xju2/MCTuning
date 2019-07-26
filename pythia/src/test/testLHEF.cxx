#include "Pythia8/Pythia.h"

#include <iostream>
#include <fstream>
#include <string.h>
#include <map>
#include <sstream>

using namespace Pythia8;
using namespace std;

// It's the same as example:main20.cc
int main(int argc, char** argv)
{
	if(argc < 2) {
		cout << argv[0] << " input.lhe" << endl;
		exit(1);
	}

	// string lhegz("amcatnlorun/events.lhe.gz");
	string lhegz(argv[1]);

	Pythia* pythia = NULL;
	pythia = new Pythia();

	LHAupLHEF *lhef = NULL;
	cout  << "HERE1 " <<   endl;
	lhef = new LHAupLHEF(&pythia->info, lhegz.c_str(), NULL, false, true);
	cout  << "HERE2 " <<   endl;

	if (!lhef->setInit()) {
		pythia->info.errorMsg("Error from LHAupMadgraph::reader: failed to "
				"initialize the LHEF reader");
		return 1;
	}
	cout  << "HERE3 " <<   endl;
	if (lhef->sizeProc() != 1) {
		pythia->info.errorMsg("Error from LHAupMadgraph::reader: number of "
				"processes is not 1"); return 1;
	}
	cout  << "HERE4 " <<   endl;

	delete lhef;
	delete pythia;
	return 0;
}
