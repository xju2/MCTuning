// -*- C++ -*-
#include "Rivet/Analysis.hh"
#include "Rivet/Projections/FinalState.hh"
#include "Rivet/Projections/FastJets.hh"
#include "Rivet/Tools/BinnedHistogram.hh"

namespace Rivet {


  /// @brief Jet mass as a funciton of ystar 
  // and chi as a function of jet mass
  class ATLAS_2017_I1519428 : public Analysis {
  public:

    /// Constructor
    // DEFAULT_RIVET_ANALYSIS_CTOR(ATLAS_2017_I1519428);

	ATLAS_2017_I1519428(): Analysis("ATLAS_2017_I1519428") {}


    /// @name Analysis methods
    //@{

    /// Book histograms and initialise projections before the run
    void init() {

      // Initialise and register projections
	  const FinalState fs;
      declare(fs, "FinalState");
      // declare(FinalState(Cuts::abseta < 5 && Cuts::pT > 100*MeV), "FS");
	  
	  FastJets fj04(fs, FastJets::ANTIKT, 0.4);
	  fj04.useInvisibles(); // what is this?
	  declare(fj04, "AntiKT04");

      // Book histograms
	  int table_index = 1;
      // _h_mass1 = bookHisto1D(1, 1, 1); // mjj with ystar < 0.6
      _h_mass2 = bookHisto1D(table_index++, 1, 1); // mjj with ystar < 1.20

	  double mjjBins[] = {3.4, 3.7, 4.0, 4.3, 4.6, 4.9, 5.4, 15};
	  for(size_t i = 0; i < 7; ++i){
		  _chi2.addHistogram(mjjBins[i], mjjBins[i+1], bookHisto1D(table_index++, 1, 1) );
	  }
    }


    /// Perform the per-event analysis
    void analyze(const Event& event) {
		Jets jetCon = apply<FastJets>(event, "AntiKT04").jetsByPt(60*GeV);

		// Identify dijets
		vector<FourMomentum> leadjets;
		foreach (const Jet& jet, jetCon) {
			if (jet.absrap() < 3.0 && leadjets.size() < 2) {
				if (leadjets.empty() && jet.pT() < 440*GeV) continue;
				leadjets.push_back(jet.momentum());
			}
		}

		// make sure we have a leading jet with pT > 440 GeV and a second to leading jet with pT > 60 GeV.
		if(leadjets.size() < 2) {
			MSG_DEBUG("Could not find two suitable leading jets");
			return;	
		}
	
		const double y1 = leadjets[0].rapidity();
		const double y2 = leadjets[1].rapidity();
		const double ystar = fabs(y1 - y2)/2.;
		const double yb = (y1 + y2)/2.;
		const double  m = (leadjets[0] + leadjets[1]).mass();
		const double chi = exp(2*ystar);

		// if(ystar < 0.6) _h_mass1->fill(m/TeV, event.weight()); // The Data not provided in HepData
		if(ystar < 1.2) _h_mass2->fill(m/TeV, event.weight());

		// chi
		if(yb < 1.1 && ystar < 1.7){
			_chi2.fill(m/TeV, chi, event.weight());
		}
    }


    /// Normalise histograms etc., after the run
    void finalize() {

      // normalize(_h_YYYY); // normalize to unity
      scale(_h_mass2, crossSection()/picobarn/sumOfWeights()); // norm to cross section
      _chi2.scale(crossSectionPerEvent()/picobarn, this); // Normalise histograms to cross section

    }

    //@}


    /// @name Histograms
    //@{
private:
    Histo1DPtr _h_mass1;
    Histo1DPtr _h_mass2;

	BinnedHistogram<double> _chi2;
    //@}


  };


  // The hook for the plugin system
  DECLARE_RIVET_PLUGIN(ATLAS_2017_I1519428);


}