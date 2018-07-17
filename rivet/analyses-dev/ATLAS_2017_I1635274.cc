// -*- C++ -*-
#include "Rivet/Analysis.hh"
#include "Rivet/Projections/FinalState.hh"
#include "Rivet/Projections/IdentifiedFinalState.hh"
#include "Rivet/Projections/VisibleFinalState.hh"
#include "Rivet/Projections/FastJets.hh"
#include "Rivet/Projections/MissingMomentum.hh"
#include "Rivet/Projections/SmearedParticles.hh"
#include "Rivet/Projections/SmearedJets.hh"
#include "Rivet/Projections/SmearedMET.hh"

namespace Rivet {

  /// @brief Add a short analysis description here
class ATLAS_2017_I1635274 : public Analysis {
public:

    /// Constructor
    DEFAULT_RIVET_ANALYSIS_CTOR(ATLAS_2017_I1635274);

    /// Book histograms and initialise projections before the run
	void init() {

		FastJets jets(FinalState(Cuts::abseta < 4.9), FastJets::ANTIKT, 0.4);
		SmearedJets recojets(jets, JET_SMEAR_ATLAS_RUN1);
		declare(recojets, "Jets");

		FinalState electrons(Cuts::abspid == PID::ELECTRON && Cuts::abseta < 2.47 && Cuts::pT > 20*GeV);
		// TODO: do use Loose for baseline electrons, but Medium for signal electrons
		SmearedParticles recoelectrons(electrons, ELECTRON_EFF_ATLAS_RUN1);
		declare(recoelectrons, "Electrons");

		FinalState muons(Cuts::abspid == PID::MUON && Cuts::abseta < 2.50 && Cuts::pT > 10*GeV);
		SmearedParticles recomuons(muons, MUON_EFF_ATLAS_RUN1);
		declare(recomuons, "Muons");

		VisibleFinalState calofs(Cuts::abseta < 4.9 && Cuts::abspid != PID::MUON);
		MissingMomentum met(calofs);
		SmearedMET recomet(met, MET_SMEAR_ATLAS_RUN1);
		declare(recomet, "MET");


		/// Book histograms
		for (size_t i = 0; i < 7; ++i)
			_count_IM[i] = bookCounter("count_IM" + toString(i+1));
		for (size_t i = 0; i < 6; ++i)
			_count_EM[i] = bookCounter("count_EM" + toString(i+1));

		//0: signal, 1: Wmunu, 2: Zmumu, 3: Wenu, 4: ttbar, 5: PreSelection
		for (size_t i = 0; i < 6; i++){
			_count_Regions[i] = bookCounter("count_Region" + toString(i+1));
		}

		int table_index = 1;
		_h_met_wmunu 	= bookHisto1D(table_index++, 1, 1);
		_h_ljetpT_wmunu = bookHisto1D(table_index++, 1, 1);
		_h_met_wenu 	= bookHisto1D(table_index++, 1, 1);
		_h_ljetpT_wenu  = bookHisto1D(table_index++, 1, 1);
		_h_met_zmumu 	= bookHisto1D(table_index++, 1, 1);
		_h_ljetpT_zmumu = bookHisto1D(table_index++, 1, 1);
		_h_met_ttbar 	= bookHisto1D(table_index++, 1, 1);
		_h_ljetpT_ttbar = bookHisto1D(table_index++, 1, 1);

		_h_met_sr 		= bookHisto1D(table_index++, 1, 1);
		_h_ljetpT_sr 	= bookHisto1D(table_index++, 1, 1);
		_h_ljeteta_sr 	= bookHisto1D(table_index++, 1, 1);
		_h_njets_sr 	= bookHisto1D(table_index++, 1, 1);

	}


	/// Perform the per-event analysis
	void analyze(const Event& event) {
		const Jets jets = apply<JetAlg>(event, "Jets").jetsByPt(Cuts::pT > 20*GeV && Cuts::abseta < 2.8);
		const Jets signal_jets = apply<JetAlg>(event, "Jets").jetsByPt(Cuts::pT > 30*GeV && Cuts::abseta < 2.8);

		const Particles elecs = apply<ParticleFinder>(event, "Electrons").particlesByPt();
		const Particles mus = apply<ParticleFinder>(event, "Muons").particlesByPt();
		MSG_DEBUG("Number of raw jets, electrons, muons = "
				<< jets.size() << ", " << elecs.size() << ", " << mus.size() << ", " << signal_jets.size());

		// Discard jets very close to electrons, or with low track multiplicity and close to muons
		const Jets isojets = filter_discard(signal_jets, [&](const Jet& j) {
				/// @todo Add track efficiency random filtering
				if (any(elecs, deltaRLess(j, 0.2))) return true;
				if (j.particles(Cuts::abscharge > 0 && Cuts::pT > 0.4*GeV).size() < 3 &&
					any(mus, deltaRLess(j, 0.4))) return true;
				return false;
				});

		// Discard electrons close to remaining jets
		const Particles isoelecs = filter_discard(elecs, [&](const Particle& e) {
				return any(isojets, deltaRLess(e, 0.4));
				});

		// Discard muons close to remaining jets
		const Particles isomus = filter_discard(mus, [&](const Particle& m) {
				for (const Jet& j : isojets) {
					if (deltaR(j,m) > 0.4) continue;
					if (j.particles(Cuts::abscharge > 0 && Cuts::pT > 0.4*GeV).size() > 3) return true;
				}
				return false;
				});

		// Calculate ETmiss
		//const Vector3& vet = apply<MissingMomentum>(event, "MET").vectorEt();
		const Vector3& vet = apply<SmearedMET>(event, "MET").vectorEt();
		const double etmiss = vet.perp();

		const double weight = event.weight();
		if(isojets.empty()) vetoEvent;

		// basic MET, jet pT, number of jets and topological cuts 
		bool topo_cuts = etmiss > 250*GeV && // met > 250 GeV
			! filter_select(isojets, Cuts::pT > 250*GeV && Cuts::abseta < 2.4).empty() && // at least one jet with pT > 250 GeV
			filter_select(isojets, Cuts::pT > 30*GeV).size() <= 4  && // at most four-jets with pT > 30 GeV
			! any(isojets, deltaPhiLess(-vet, 0.4));

		if(!topo_cuts) vetoEvent;

		int region_ = 5;

		//signal regions
		if ( isoelecs.empty() && isomus.empty())
		{
			region_ = 0;
			// Get ETmiss bin number and fill counters
			const int i_etmiss = binIndex(etmiss/GeV, ETMISS_CUTS);
			// Inclusive ETmiss bins
			for (int ibin = 0; ibin < 7; ++ibin)
				if (i_etmiss >= ibin) _count_IM[ibin]->fill(weight);
			// Exclusive ETmiss bins
			if (inRange(i_etmiss, 0, 6)) _count_EM[i_etmiss]->fill(weight);

			// Fill histograms
			_h_met_sr		->fill(etmiss/GeV, weight);
			_h_ljetpT_sr	->fill(isojets[0].pt()/GeV, weight);
			_h_ljeteta_sr	->fill(isojets[0].eta(), weight);
			_h_njets_sr		->fill(isojets.size(), weight);
		}

		Jets bjets;
		foreach( const Jet& j, signal_jets) {
			if( j.bTagged() && 
				rand()/static_cast<double>(RAND_MAX) < JET_BTAG_ATLAS_RUN1(j)) {
				bjets.push_back(j);
			}
		}

		// W -> munu control region
		if (isomus.size() == 1 && isoelecs.empty())
		{
			const Particle& muon = isomus.at(0);
			double mT = sqrt(2* muon.perp() * etmiss * (1 - cos(deltaPhi(muon.phi(), vet.phi())) ));
			if( mT > 30*GeV && mT < 100*GeV){
				if(bjets.empty()){
					region_ = 1;
					_h_met_wmunu	->fill(etmiss/GeV, weight);
					_h_ljetpT_wmunu	->fill(isojets[0].pt()/GeV, weight);
				} else {
					region_ = 4;
					_h_met_ttbar	->fill(etmiss/GeV, weight);
					_h_ljetpT_ttbar	->fill(isojets[0].pt()/GeV, weight);
				}
			}
		}

		// Z -> mumu control region
		if( isomus.size() == 2 && isoelecs.empty())
		{
			const double m = (isomus[0].momentum() + isomus[1].momentum()).mass();
			if(m > 66*GeV && m < 116*GeV){
				region_ = 2;
				_h_met_zmumu	->fill(etmiss/GeV, weight);
				_h_ljetpT_zmumu	->fill(isojets[0].pt()/GeV, weight);
			}
		}

		// W -> enu control region
		if(isoelecs.size() == 1){
			// and with pT > 30 GeV and 1.37 < |eta| < 1.52
			const Particle& ele = isoelecs.at(0);
			if(ele.pt() > 30*GeV && (ele.eta() <= 1.37 || ele.eta() >= 1.52) )
			{
				double HT = 0;
				foreach(const Jet& j, isojets){
					HT += j.pT();
				}
				double ht_ratio = etmiss/sqrt(HT);
				double mT = sqrt(2* ele.perp() * etmiss * (1 - cos(deltaPhi(ele.phi(), vet.phi())) ));
				if( mT > 30*GeV && mT < 100*GeV && ht_ratio > 5){
					region_ = 3;
					_h_met_wenu		->fill(etmiss/GeV, weight);
					_h_ljetpT_wenu	->fill(isojets[0].pt()/GeV, weight);
				}
			}
		}
		_count_Regions[region_]->fill(weight);
	}

	/// Normalise histograms etc., after the run
	void finalize() {
		const double norm = 36.1*crossSection()/femtobarn;
		scale(_count_IM, 		norm/sumOfWeights());
		scale(_count_EM, 		norm/sumOfWeights());
		scale(_count_Regions, 	norm/sumOfWeights());

		scale(_h_met_wmunu, 	norm/sumOfWeights()); // norm to cross section
		scale(_h_ljetpT_wmunu, 	norm/sumOfWeights()); // norm to cross section
		scale(_h_met_zmumu, 	norm/sumOfWeights()); // norm to cross section
		scale(_h_ljetpT_zmumu, 	norm/sumOfWeights()); // norm to cross section
		scale(_h_met_wenu, 		norm/sumOfWeights()); // norm to cross section
		scale(_h_ljetpT_wenu, 	norm/sumOfWeights()); // norm to cross section
		scale(_h_met_ttbar, 	norm/sumOfWeights()); // norm to cross section
		scale(_h_ljetpT_ttbar, 	norm/sumOfWeights()); // norm to cross section

		scale(_h_met_sr, 		norm/sumOfWeights()); // norm to cross section
		scale(_h_ljetpT_sr, 	norm/sumOfWeights()); // norm to cross section
		scale(_h_ljeteta_sr, 	norm/sumOfWeights()); // norm to cross section
		scale(_h_njets_sr, 		norm/sumOfWeights()); // norm to cross section
	}

private:
	Histo1DPtr _h_met_wmunu;
	Histo1DPtr _h_ljetpT_wmunu;
	Histo1DPtr _h_met_wenu;
	Histo1DPtr _h_ljetpT_wenu;
	Histo1DPtr _h_met_zmumu;
	Histo1DPtr _h_ljetpT_zmumu;
	Histo1DPtr _h_met_ttbar;
	Histo1DPtr _h_ljetpT_ttbar;

	Histo1DPtr _h_met_sr;
	Histo1DPtr _h_ljetpT_sr;
	Histo1DPtr _h_ljeteta_sr;
	Histo1DPtr _h_njets_sr;

	const vector<double> ETMISS_CUTS = {250, 300, 350, 400, 500, 600, 700, 13000};
	CounterPtr _count_IM[7], _count_EM[6];
	CounterPtr _count_Regions[6];
	double region_count[5] = {0, 0, 0, 0, 0};
};


// The hook for the plugin system
DECLARE_RIVET_PLUGIN(ATLAS_2017_I1635274);
}
