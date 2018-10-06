#include "Pythia8/Pythia.h"

#include <iostream>
#include <fstream>
#include <string.h>
#include <map>

using namespace Pythia8;
using namespace std;

// It's the same as example:main20.cc
int main(int argc, char** argv){
	// Check that correct number of command-line arguments
	if (argc < 3) {
		cerr << " Unexpected number of command-line arguments. \n You are"
			<< " expected to provide one input and one output file name. \n"
			<< " Program stopped! " << endl;
		return 1;
	}

	// Check that the provided input name corresponds to an existing file.
	ifstream is(argv[1]);
	if (!is) {
		cerr << " Command-line file " << argv[1] << " was not found. \n"
			<< " Program stopped! " << endl;
		return 1;
	}

	// Confirm that external files will be used for input and output.
	cout << "\n >>> PYTHIA settings will be read from file " << argv[1]
		<< " <<< \n >>> HepMC events will be written to file "
		<< argv[2] << " <<< \n" << endl;

	bool debug = true;

	// Generator.
	Pythia pythia;

	// Read in commands from external file.
	pythia.readFile(argv[1]);
	pythia.init();

	pythia.settings.listChanged();

	// weight strings
	vector<string> weightStrings = pythia.settings.wvec("UncertaintyBands:List");
	if(debug){
		cout << "Weight strings: |" ;
		for(auto& ws : weightStrings){
			cout << ws << "|";
		}
		cout << endl;
	}

	// Create an LHAup object that can access relevant information in pythia.
	// LHAupFromPYTHIA8 myLHA(&pythia.process, &pythia.info);
	LHEF3FromPythia8 myLHA(&pythia.event, &pythia.settings, &pythia.info,
    &pythia.particleData);

	// Open a file on which LHEF events should be stored, and write header.
	myLHA.openLHEF(argv[2]);

	// Store initialization info in the LHAup object.
	myLHA.setInit();

	// Write out this initialization info on the file.
	// myLHA.initLHEF();

	int nEvent = pythia.mode("Main:numberOfEvents");
	if (pythia.mode("Beams:frameType") == 4) {
		nEvent = 1000000000;
	}
	int nAbort = pythia.mode("Main:timesAllowErrors");

	// Loop over events
	int iAbort = 0;
	for(int iEvent = 0; iEvent < nEvent; ++iEvent) {

		// generate event
		if(!pythia.next()) {
			if (pythia.info.atEndOfFile()) {
				cout << " Aborted since reached end of Les Houches Event File\n";
				break;
			}

			if(++iAbort < nAbort) continue;
			cout << " Event generation aborted prematurely, owing to error!\n";
			break;
		}
		if(debug) {
			cout << "LHEF version: " << pythia.info.LHEFversion() << endl;
		}

		// print out event weights
		double phaseSpaceWeight = pythia.info.weight();
		double mergingWeight    = pythia.info.mergingWeight();
		double eventWeight = phaseSpaceWeight*mergingWeight;
		if(debug) {
			cout << "Event weights: phase space weight: " << phaseSpaceWeight <<
				" mergingWeight: " << mergingWeight <<
				" eventWeight: " << eventWeight << endl;
		}
		if(pythia.info.getWeightsDetailedSize() != 0){
			for(std::map<string, Pythia8::LHAwgt>:: const_iterator wgt = pythia.info.rwgt->wgts.begin();
					wgt != pythia.info.rwgt->wgts.end(); ++wgt) {
				if(debug) {
					cout <<"Found: " << wgt->first << " " <<endl;
				}
			}
		} else{
			if(debug) {
				cout << "No detailed format" << endl;
			}
		}
		
		if(debug) cout << "List " << pythia.info.nWeights() << " weights: " ;
		for(int iw = 0; iw != pythia.info.nWeights(); ++iw) {
			if(debug) {
				cout << pythia.info.weight(iw) << ", ";
			}
		}
		cout << endl;

		// Store event info in the LHAup object.
		myLHA.setEvent();
		// Write out this event info on the file.
		// With optional argument (verbose =) false the file is smaller.
		// myLHA.eventLHEF();

	}
	// Statistics: full printout.
	pythia.stat();

	// Update the cross section info based on Monte Carlo integration during run.
	// myLHA.updateSigma();

	// Write endtag. Overwrite initialization info with new cross sections.
	myLHA.closeLHEF(true);


	return 0;
}
