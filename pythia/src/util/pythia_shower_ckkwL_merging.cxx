#include "Pythia8/Pythia.h"
#include "Pythia8Plugins/HepMC2.h"

#include <iostream>
#include <fstream>
#include <string.h>

using namespace Pythia8;
using namespace std;

// It's the same as example:main42.cc
int main(int argc, char** argv) {

  // Check that correct number of command-line arguments
  if (argc < 4) {
    cerr << " pythia_shower_ckkwL_merging pythia.cmd in.lhe.gz out.hepmc\n" << endl;
    return 1;
  }

  // Confirm that external files will be used for input and output.
  cout << "\n >>> PYTHIA settings will be read from file " << argv[1]
       << " <<< \n >>> HepMC events will be written to file "
       << argv[3] << " <<< \n" << endl;


	// Set up pythia to hepmc object
	HepMC::Pythia8ToHepMC ToHepMC;
	HepMC::IO_GenEvent ascii_io(argv[3], std::ios::out);

	// Switch off warnings for parton-level events.
	ToHepMC.set_free_parton_exception(false);

	// Generator
	Pythia pythia;
	// Initialize Les Houches Event File run. List initialization information.
	pythia.readFile(argv[1]);
	// pythia.readString("Beams:frameType = 4");
	pythia.readString("Beams:LHEF = "+string(argv[2]));


	Event& event = pythia.event;

	// check other options
	const char* tune_file = NULL;
	for(int i = 3; i < argc; i++){
		const char* key = strtok(argv[i], "=");
		const char* val = strtok(NULL, " ");
		if(strcmp(key, "-t") == 0) tune_file = val;
	}
	if(tune_file){
		cout <<" tunning file: " << tune_file << endl;
		pythia.readFile(tune_file);
	}else{
		cout << "no tunning file" << endl;
	}

	int nEvent = pythia.mode("Main:numberOfEvents");
	if (pythia.mode("Beams:frameType") == 4) {
		nEvent = 1000000000;
	}
	int nAbort = pythia.mode("Main:timesAllowErrors");

	pythia.init();
	
	// show settings
	// pythia.settings.listChanged();
	// pythia.particleData.listChanged();

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

		HepMC::GenEvent* hepmcevt = new HepMC::GenEvent();
		double evtweight = pythia.info.weight();
		double weight = pythia.info.mergingWeight();
		if(iEvent < 5) {
			cout <<  " event weight: " << evtweight <<" merging weight: " << weight << endl;
		}
		//weight *= evtweight;
		//hepmcevt->weights().push_back(weight);
		ToHepMC.fill_next_event(pythia, hepmcevt);

		// write the HepMC event to file. Done with it.
		ascii_io << hepmcevt;
		delete hepmcevt;
	}

	pythia.stat();
	return 0;
}
