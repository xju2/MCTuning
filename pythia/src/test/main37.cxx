// main37.cc is a part of the PYTHIA event generator.
// Copyright (C) 2018 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL v2 or later, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// This is a simple test program.
// It illustrates how Les Houches Event File version 3.0 input can be used
// to mix events according to several different event weights.

#include "Pythia8/Pythia.h"
#include<iostream>

using namespace Pythia8;
using namespace std;

int main(int argc, char** argv) 
{
	if (argc < 2) {
		cerr << " Unexpected number of command-line arguments. \n You are"
			<< " expected to provide one LHE3 input. \n"
			<< " Program stopped! " << endl;
		return 1;
	}

  // Generator
  Pythia pythia;


  // Initialize Les Houches Event File run.
  pythia.readString("Beams:frameType = 4");
  pythia.readString("Beams:LHEF = "+string(argv[1]));
  pythia.readString("Print::quiet = on");
  pythia.init();

  // Get number of event weights.
  int ninitrwgt = 0;
  if (pythia.info.initrwgt) ninitrwgt = pythia.info.initrwgt->size();
  cout <<"n init wgt: " << ninitrwgt << endl;
  cout <<"n init wgt groups: " << pythia.info.initrwgt->sizeWeightGroups() << endl;
  cout << "KEY: ";
  for(auto& key: pythia.info.initrwgt->weightsKeys) {
	  cout <<"\t>> " << key << endl;
  }
  map<string, LHAweight>& init_weights = pythia.info.initrwgt->weights;
  for(auto& weight_info: init_weights) {
	  cout << weight_info.first << ", " << weight_info.second.id << ", " << weight_info.second.contents << endl;
  }

  // Begin event loop; generate until none left in input file.
  for (int iEvent = 0; iEvent < 1; ++iEvent) {

    // Generate events, and check whether generation failed.
    if (!pythia.next()) {
      // If failure because reached end of file then exit event loop.
      if (pythia.info.atEndOfFile()) break;
      else continue;
    }

    // Loop over the event weights in the detailed format and histogram.
    int iwgt = 0;
    map<string,double> weights;
    if ( pythia.info.weights_detailed ) {
		weights = *(pythia.info.weights_detailed);
	}
	cout << "total weights: " << weights.size() << endl;
    for (auto it = weights.begin(); it != weights.end(); ++it ) {
		cout <<it->first << " " << it->second << endl;
	}

  // End of event loop.
  }

  // Done.
  return 0;
}
