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
#include "Rivet/Tools/Logging.hh"

#include <stdio.h>

namespace Rivet {

  /// @brief Add a short analysis description here
class ATLAS_2017_I1635274 : public Analysis {
public:

    /// Constructor
    DEFAULT_RIVET_ANALYSIS_CTOR(ATLAS_2017_I1635274);


    /// Book histograms and initialise projections before the run
	void init() {
		use_truth = false;
		int print_level = Log::INFO;
		this->getLog().setLevel(Log::INFO);

		FastJets jets(FinalState(Cuts::abseta < 4.9), FastJets::ANTIKT, 0.4);
		SmearedJets recojets(jets, JET_SMEAR_ATLAS_RUN1);
		declare(recojets, "JetsReco");

		IdentifiedFinalState elecs(Cuts::abseta < 2.47 && Cuts::pT > 20*GeV);
		elecs.acceptIdPair(PID::ELECTRON);
		declare(elecs, "ElectronTruth");
		IdentifiedFinalState muons(Cuts::abseta < 2.5 && Cuts::pT > 10*GeV);
		muons.acceptIdPair(PID::MUON);
		declare(muons, "MuonTruth");
		
		// SmearedParticles recoEle(elecs, ELECTRON_EFF_ATLAS_RUN1, ELECTRON_SMEAR_ATLAS_RUN1);
		SmearedParticles recoEle(elecs, ELECTRON_IDEFF_ATLAS_RUN2_LOOSE, ELECTRON_SMEAR_ATLAS_RUN1);
		recoEle.getLog().setLevel(print_level);
		declare(recoEle, "ElectronReco");

		SmearedParticles recoMuons(muons, MUON_EFF_ATLAS_RUN1, MUON_SMEAR_ATLAS_RUN1);
		recoMuons.getLog().setLevel(print_level);
		declare(recoMuons, "MuonReco");

		// Missing ET for all objects
		VisibleFinalState calofs(Cuts::abseta < 4.9);
		MissingMomentum met(calofs);
		declare(met, "METTruth");
		met.getLog().setLevel(print_level);
		SmearedMET recomet(met, MET_SMEAR_ATLAS_RUN1);
		recomet.getLog().setLevel(print_level);
		declare(recomet, "METReco");


		// Missing ET without Muon's contribution
		VisibleFinalState calofsNoMuon(Cuts::abseta < 4.9 && Cuts::abspid != PID::MUON);
		MissingMomentum metNoMuon(calofsNoMuon);
		declare(metNoMuon, "METNoMuonTruth");
		metNoMuon.getLog().setLevel(print_level);
		SmearedMET recometNoMuon(metNoMuon, MET_SMEAR_ATLAS_RUN1);
		recometNoMuon.getLog().setLevel(print_level);
		declare(recometNoMuon, "METNoMuonReco");


		// Missing ET without Muon nor Electron's contribution
		VisibleFinalState calofsNoEle(Cuts::abseta < 4.9 && Cuts::abspid != PID::MUON && Cuts::abspid != PID::ELECTRON);
		MissingMomentum metNoEle(calofsNoEle);
		declare(metNoEle, "METNoEleTruth");
		SmearedMET recometNoEle(metNoEle, MET_SMEAR_ATLAS_RUN1);
		recometNoEle.getLog().setLevel(print_level);
		declare(recometNoEle, "METNoEleReco");


		/// Book histograms
		for (size_t i = 0; i < 7; ++i)
			_count_IM[i] = bookCounter("count_IM" + toString(i+1));
		for (size_t i = 0; i < 6; ++i)
			_count_EM[i] = bookCounter("count_EM" + toString(i+1));

		//0: signal, 1: Wmunu, 2: Zmumu, 3: Wenu, 4: ttbar, 5: PreSelection
		for (size_t i = 0; i < 6; i++){
			_count_Regions[i] = bookCounter("count_Region" + toString(i));
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

		_h_MET_Truth = bookHisto1D("MET_Truth", 100, 0, 1000);
		_h_MET_Reco = bookHisto1D("MET_Reco", 100, 0, 1000);
		_h_METNoEle_Truth = bookHisto1D("METNoEle_Truth", 100, 0, 1000);
		_h_METNoEle_Reco = bookHisto1D("METNoEle_Reco", 100, 0, 1000);

		_h_dphiNorm = bookHisto1D("dPhiNorm", 10, 0, Rivet::pi);
		_h_dphiEle	= bookHisto1D("dPhiEle", 10, 0, Rivet::pi);
		_h_n_ele 	= bookHisto1D("nElectrons", 10, 0, 10);
		_h_n_jets	= bookHisto1D("nJets", 10, 0, 10);
		_h_n_mu		= bookHisto1D("nMuons", 10, 0, 10);
		_h_n_muNoRM	= bookHisto1D("nMuonsNoRM", 10, 0, 10);
		_h_enu_mT	= bookHisto1D("enu_mT", 40, 0, 200);
		_h_munu_mT	= bookHisto1D("munu_mT", 40, 0, 200);
		_h_mZ		= bookHisto1D("mZ", 40, 0, 200);
		_h_n_bjets	= bookHisto1D("nBJets", 10, 0, 10);
	}


	/// Perform the per-event analysis
	void analyze(const Event& event) 
	{
		MSG_DEBUG("Entering...");
		const double weight = event.weight();

		event.getLog().setLevel(Log::INFO);
		const Jets signal_jets  = apply<JetAlg>(event, "JetsReco").jetsByPt(Cuts::pT > 30*GeV && Cuts::abseta < 2.8);

		const Particles eleTruth = apply<IdentifiedFinalState>(event, "ElectronTruth").particlesByPt();
		const Particles eleReco  = apply<ParticleFinder>(event, "ElectronReco").particlesByPt();

		const Particles muTruth  = apply<IdentifiedFinalState>(event, "MuonTruth").particlesByPt();
		const Particles muReco   = apply<ParticleFinder>(event, "MuonReco").particlesByPt();

		MSG_DEBUG("Truth muons: " << muTruth.size() << ", Reco muons: " << muReco.size());
		MSG_DEBUG("Truth electrons: " << eleTruth.size() << ", Reco electrons: " << eleReco.size());
		MSG_DEBUG("Number of raw jets, electrons, muons, leptons = "
				<< signal_jets.size() << ", " << eleTruth.size() << ", " << muTruth.size());

		const Particles& elecs = use_truth? eleTruth: eleReco;
		const Particles& mus   = use_truth? muTruth: muReco;

		_h_n_muNoRM->fill(mus.size(), weight);

		// Discard jets very close to electrons, or with low track multiplicity and close to muons
		const Jets isojets = filter_discard(signal_jets, [&](const Jet& j) {
				/// @todo Add track efficiency random filtering
				if (any(elecs, deltaRLess(j, 0.2))) return true;
				if (j.particles(Cuts::abscharge > 0 && Cuts::pT > 0.4*GeV).size() <= 3 &&
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
					// if (j.particles(Cuts::abscharge > 0 && Cuts::pT > 0.4*GeV).size() > 3) return true;
				}
				return false;
				});

		// ETmiss with all objects	
		const Vector3& vetTruth = apply<MissingMomentum>(event, "METTruth").vectorMissingEt();
		const Vector3& vetReco  = apply<SmearedMET>(event, "METReco").vectorMissingEt();
		const Vector3& vet = use_truth? vetTruth: vetReco;
		const double etmiss = vet.perp();
		// _h_MET_Truth->fill(vetTruth.mod()/GeV, weight);
		// _h_MET_Reco ->fill(vet.mod()/GeV, weight);

		// ETmiss with no Muon's contribution
		const Vector3& vetNoMuonTruth = apply<MissingMomentum>(event, "METNoMuonTruth").vectorMissingEt();
		const Vector3& vetNoMuonReco  = apply<SmearedMET>(event, "METNoMuonReco").vectorMissingEt();
		_h_MET_Truth->fill(vetNoMuonTruth.mod()/GeV, weight);
		_h_MET_Reco ->fill(vetNoMuonReco.mod()/GeV, weight);

		const Vector3& vetNoMuon = use_truth? vetNoMuonTruth: vetNoMuonReco;
		const double etmissNoMuon = vetNoMuon.perp();

		// ETmissing with no Muon nor Electron's contribution

		const Vector3& vetNoEleTruth = apply<MissingMomentum>(event, "METNoEleTruth").vectorMissingEt();
		const Vector3& vetNoEleReco  = apply<SmearedMET>(event, "METNoEleReco").vectorMissingEt();
		_h_METNoEle_Truth->fill(vetNoEleTruth.mod()/GeV, weight);
		_h_METNoEle_Reco ->fill(vetNoEleReco.mod()/GeV, weight);

		const Vector3& vetNoEle = use_truth? vetNoEleTruth: vetNoEleReco;
		const double etmissNoEle = vetNoEle.perp();

		if(isojets.empty()) vetoEvent;

		// basic MET, jet pT, number of jets and topological cuts 
		double min_dphi_norm = 99;
		double min_dphi_ele = 99;
		for(const Jet& j: isojets) {
			double dphi = fabs(mapAngle(deltaPhi(j, vetNoMuon), MINUSPI_PLUSPI));
			if (dphi < min_dphi_norm) min_dphi_norm = dphi;

			dphi = fabs(mapAngle(deltaPhi(j, vetNoEle), MINUSPI_PLUSPI));
			if (dphi < min_dphi_ele) min_dphi_ele = dphi;
		}

		bool dphi_norm = min_dphi_norm > 0.4;
		bool dphi_ele  = min_dphi_ele > 0.4;

		// btagging
		Jets bjets;
		foreach( const Jet& j, signal_jets) {
			if( j.bTagged() ){ 
				if(use_truth || rand()/static_cast<double>(RAND_MAX) < JET_BTAG_ATLAS_RUN1(j) ) {
					bjets.push_back(j);
				}
			}
		}

		// Fill Histograms for debugging
		_h_n_ele	->fill(isoelecs.size(), weight);
		_h_n_jets	->fill(isojets.size(), 	weight);
		_h_n_mu		->fill(isomus.size(), 	weight);
		_h_n_bjets	->fill(bjets.size(), 	weight);
		_h_dphiNorm	->fill(min_dphi_norm, 	weight);
		_h_dphiEle	->fill(min_dphi_ele,	weight);

		bool topo_cuts = (etmissNoMuon > 250*GeV || etmissNoEle > 250*GeV) && // met > 250 GeV
			! filter_select(isojets, Cuts::pT > 250*GeV && Cuts::abseta < 2.4).empty() && // at least one jet with pT > 250 GeV
			filter_select(isojets, Cuts::pT > 30*GeV).size() <= 4  && // at most four-jets with pT > 30 GeV
			(dphi_norm || dphi_ele);

		if(!topo_cuts) vetoEvent;

		MSG_DEBUG("Number of filtered jets, electrons, muons = "
				<< isojets.size() << ", " << isoelecs.size() << ", " << isomus.size());
		double leading_jet_pt = isojets[0].pt();
		double leading_jet_eta = isojets[0].abseta();

		int region_ = 5;
		MSG_DEBUG("Passed basic selections");

		//signal regions
		if ( etmissNoMuon > 250*GeV && dphi_norm && isoelecs.empty() && isomus.empty())
		{
			MSG_DEBUG("No electrons, no Muons...");
			region_ = 0;
			// Get ETmiss bin number and fill counters
			const int i_etmiss = binIndex(etmissNoMuon/GeV, ETMISS_CUTS);
			// Inclusive ETmiss bins
			for (int ibin = 0; ibin < 7; ++ibin)
				if (i_etmiss >= ibin) _count_IM[ibin]->fill(weight);
			// Exclusive ETmiss bins
			if (inRange(i_etmiss, 0, 6)) _count_EM[i_etmiss]->fill(weight);

			// Fill histograms
			_h_met_sr		->fill(etmissNoMuon/GeV, weight);
			_h_ljetpT_sr	->fill(leading_jet_pt/GeV, weight);
			_h_ljeteta_sr	->fill(leading_jet_eta, weight);
			_h_njets_sr		->fill(isojets.size(), weight);
		}


		// W -> munu control region
		if (etmissNoMuon > 250*GeV && dphi_norm && isomus.size() == 1 && isoelecs.empty())
		{
			MSG_DEBUG("Having one muon...");
			const Particle& muon = isomus.at(0);
			double mT = sqrt(2* muon.pt() * etmiss * (1 - cos(deltaPhi(muon, vet)) ));
			MSG_DEBUG("mT:" << mT/GeV << "[GeV], bjets size:"
					<< bjets.size()<<"; MET: " << etmiss/GeV << " [GeV]. Muon pT: " 
					<< muon.pt()/GeV << " [GeV]; DeltaPhi: " <<  deltaPhi(muon, vet)
					); 
			_h_munu_mT	->fill(mT/GeV, weight);
			if( mT > 30*GeV && mT < 100*GeV){
				if(bjets.empty()){
					region_ = 1;
					_h_met_wmunu	->fill(etmissNoMuon/GeV, weight);
					_h_ljetpT_wmunu	->fill(leading_jet_pt/GeV, weight);
				} else {
					//ttbar
					region_ = 4;
					_h_met_ttbar	->fill(etmissNoMuon/GeV, weight);
					_h_ljetpT_ttbar	->fill(leading_jet_pt/GeV, weight);
				}
			}
		}

		// Z -> mumu control region
		if( etmissNoMuon > 250* GeV && dphi_norm && isomus.size() == 2 && isoelecs.empty())
		{
			const double m = (isomus[0].momentum() + isomus[1].momentum()).mass();
			_h_mZ->fill(m/GeV, weight);
			if(m > 66*GeV && m < 116*GeV){
				region_ = 2;
				_h_met_zmumu	->fill(etmissNoMuon/GeV, weight);
				_h_ljetpT_zmumu	->fill(leading_jet_pt/GeV, weight);
			}
		}

		// W -> enu control region
		if(etmissNoEle > 250*GeV && dphi_ele && isoelecs.size() == 1 && isomus.empty())
		{
			MSG_DEBUG("Having one electron...");
			// and with pT > 30 GeV and 1.37 < |eta| < 1.52
			const Particle& ele = isoelecs.at(0);
			if(ele.pt() > 30*GeV && (ele.abseta() <= 1.37 || ele.abseta() >= 1.52) )
			{
				double HT = 0;
				foreach(const Jet& j, isojets){
					HT += j.pT();
				}
				double mT = sqrt(2* ele.pt() * etmiss * (1 - cos(deltaPhi(ele, vet)) ));
				double ht_ratio = etmissNoMuon/sqrt(HT);
				MSG_DEBUG("mT:" << mT/GeV << "[GeV], " << ht_ratio 
						<< ". MET " << etmissNoMuon/GeV << " MET-NO-ELE: " << etmissNoEle/GeV 
						<< ". Leading jet pT: " << leading_jet_pt/GeV); 
				_h_enu_mT	->fill(mT/GeV, weight);
				if( mT > 30*GeV && mT < 100*GeV && ht_ratio > 5){
					region_ = 3;
					_h_met_wenu		->fill(etmissNoEle/GeV, weight);
					_h_ljetpT_wenu	->fill(leading_jet_pt/GeV, weight);
				}
			}
		}
		_count_Regions[region_]->fill(weight);
		MSG_DEBUG("Leaving...");
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

		scale(_h_MET_Truth, 	norm/sumOfWeights());
		scale(_h_MET_Reco, 		norm/sumOfWeights());
		scale(_h_METNoEle_Truth, 	norm/sumOfWeights());
		scale(_h_METNoEle_Reco, 	norm/sumOfWeights());

		scale(_h_dphiNorm,		norm/sumOfWeights());
		scale(_h_dphiEle,		norm/sumOfWeights());
		scale(_h_n_ele,			norm/sumOfWeights());
		scale(_h_n_jets,		norm/sumOfWeights());
		scale(_h_n_mu,			norm/sumOfWeights());
		scale(_h_n_muNoRM,		norm/sumOfWeights());
		scale(_h_enu_mT,		norm/sumOfWeights());
		scale(_h_munu_mT,		norm/sumOfWeights());
		scale(_h_mZ,			norm/sumOfWeights());
		scale(_h_n_bjets,		norm/sumOfWeights());
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

	Histo1DPtr _h_MET_Truth;
	Histo1DPtr _h_MET_Reco;
	Histo1DPtr _h_METNoEle_Truth;
	Histo1DPtr _h_METNoEle_Reco;

	Histo1DPtr _h_dphiNorm;
	Histo1DPtr _h_dphiEle;
	Histo1DPtr _h_n_ele;
	Histo1DPtr _h_n_jets;
	Histo1DPtr _h_n_mu;
	Histo1DPtr _h_n_muNoRM;
	Histo1DPtr _h_enu_mT;
	Histo1DPtr _h_munu_mT;
	Histo1DPtr _h_mZ;
	Histo1DPtr _h_n_bjets;


	const vector<double> ETMISS_CUTS = {250, 300, 350, 400, 500, 600, 700, 13000};
	CounterPtr _count_IM[7], _count_EM[6];
	CounterPtr _count_Regions[6];
	double region_count[5] = {0, 0, 0, 0, 0};
	bool use_truth;
};


// The hook for the plugin system
DECLARE_RIVET_PLUGIN(ATLAS_2017_I1635274);
}
