#include "Pythia8/Pythia.h"
#include "Pythia8Plugins/HepMC2.h"

#include <iostream>

using namespace Pythia8;
using namespace std;

int main() {

	// Set up pythia to hepmc object
	HepMC::Pythia8ToHepMC ToHepMC;
	HepMC::IO_GenEvent ascii_io("test.dat", std::ios::out);
	// Generator
	Pythia pythia;

	Event& event = pythia.event;

	// Read in commands from external file.
	pythia.readFile("pythia_run.cmnd");

	int nEvent = pythia.mode("Main:numberOfEvents");
	int nAbort = pythia.mode("Main:timesAllowErrors");

	pythia.init();
	
	// show settings
	pythia.settings.listChanged();
	pythia.particleData.listChanged();

	// Loop over events
	int iAbort = 0;
	for(int iEvent = 0; iEvent < nEvent; ++iEvent) {
		if(!pythia.next()) {
			if(++iAbort < nAbort) continue;
			cout << " Event generation aborted prematurely, owing to error!\n";
		}

		HepMC::GenEvent* hepmcevt = new HepMC::GenEvent();
		ToHepMC.fill_next_event(pythia, hepmcevt);
		ascii_io << hepmcevt;
		delete hepmcevt;
	}

	pythia.stat();
	return 0;
}
