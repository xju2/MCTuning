// -*- C++ -*-
#include "Rivet/Analysis.hh"
#include "Rivet/Projections/FinalState.hh"
#include "Rivet/Projections/FastJets.hh"
#include "Rivet/Tools/BinnedHistogram.hh"

#include "Rivet/Projections/SmearedParticles.hh"
#include "Rivet/Projections/SmearedJets.hh"

#include <stdio.h>

namespace Rivet {


  /// @brief Jet mass as a funciton of ystar 
  // and chi as a function of jet mass
  class ATLAS_2017_I1519428 : public Analysis {
  public:

    /// Constructor
    DEFAULT_RIVET_ANALYSIS_CTOR(ATLAS_2017_I1519428);

    /// @name Analysis methods
    //@{

    /// Book histograms and initialise projections before the run
    void init() {

      // Initialise and register projections
	  const FinalState calofs(Cuts::abseta < 4.9 && Cuts::pT > 100*MeV);
      declare(calofs, "Clusters");
	  
	  FastJets fj04(calofs, FastJets::ANTIKT, 0.4);
	  // fj04.useInvisibles(); // what is this?
	  declare(fj04, "TruthJets");

	  // smearing functions
	  declare(SmearedJets(fj04, JET_SMEAR_ATLAS_RUN1), "Jets");

      // Book histograms
	  int table_index = 1;
      _h_mass1 = bookHisto1D(table_index++, 1, 1); // mjj with ystar < 0.6
      _h_mass2 = bookHisto1D(table_index++, 1, 1); // mjj with ystar < 1.20

	  double mjjBins[] = {3.4, 3.7, 4.0, 4.3, 4.6, 4.9, 5.4, 15};
	  for(size_t i = 0; i < 7; ++i){
		  _chi2.addHistogram(mjjBins[i], mjjBins[i+1], bookHisto1D(table_index++, 1, 1) );
	  }
    }


    /// Perform the per-event analysis
    void analyze(const Event& event) {

		const double LEADING_JET_PT_CUT = 440*GeV;

		// Identify dijets
		Jets jetCon = apply<JetAlg>(event, "Jets").jetsByPt(Cuts::pT > 60*GeV);

		vector<FourMomentum> leadjets;
		foreach (const Jet& jet, jetCon) {
			if (leadjets.size() < 2) {
				if (leadjets.empty() && jet.pT() < LEADING_JET_PT_CUT) continue;
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

		if(ystar < 0.6) _h_mass1->fill(m/TeV, event.weight());
		if(ystar < 1.2) _h_mass2->fill(m/TeV, event.weight());

		// chi
		if(yb < 1.1 && ystar < 1.7){
			_chi2.fill(m/TeV, chi, event.weight());
		}
    }


    /// Normalise histograms etc., after the run
    void finalize() {

      scale(_h_mass1, crossSection()/picobarn/sumOfWeights()); // norm to cross section
      scale(_h_mass2, crossSection()/picobarn/sumOfWeights()); // norm to cross section
    
	  // normalize to unity
	  foreach (Histo1DPtr hist, _chi2.getHistograms()){
		  normalize(hist);
	  }

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
