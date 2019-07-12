// -*- C++ -*-
#include "Rivet/Analysis.hh"
#include "Rivet/Projections/FinalState.hh"
#include "Rivet/Projections/IdentifiedFinalState.hh"
#include "Rivet/Projections/VisibleFinalState.hh"
#include "Rivet/Projections/FastJets.hh"
#include "Rivet/Projections/DressedLeptons.hh"
#include "Rivet/Projections/MissingMomentum.hh"
#include "Rivet/Projections/SmearedParticles.hh"
#include "Rivet/Projections/SmearedJets.hh"
#include "Rivet/Projections/SmearedMET.hh"
#include "Rivet/Tools/Logging.hh"

#include <stdio.h>
#include <TFile.h>

namespace Rivet {

  /// @brief Add a short analysis description here
class ATLAS_2017_I1599399 : public Analysis {
public:

    /// Constructor
    DEFAULT_RIVET_ANALYSIS_CTOR(ATLAS_2017_I1599399);


    /// Book histograms and initialise projections before the run
	void init() {
		// this->getLog().setLevel(Log::INFO);
		int print_level = Log::DEBUG;
		this->getLog().setLevel(Log::DEBUG);

		// Jets
		FinalState fs(Cuts::abseta < 5.0);
		FastJets jets(fs, FastJets::ANTIKT, 0.4);
		declare(jets, "JetsTruth");
		// Smearing Jets
		SmearedJets recojets(jets, JET_SMEAR_ATLAS_RUN1); // replace the smearing function
		declare(recojets, "JetsReco");

		// Bare Leptons
		IdentifiedFinalState photons(fs);
		photons.acceptIdPair(PID::PHOTON);

		IdentifiedFinalState bare_el(fs);
		bare_el.acceptIdPair(PID::ELECTRON);
		declare(bare_el, "bareElectrons");

		IdentifiedFinalState bare_mu(fs);
		bare_mu.acceptIdPair(PID::MUON);
		declare(bare_mu, "bareMuons");

		Cut ele_cuts = Cuts::abseta < 2.47 && Cuts::pT > 20*GeV;
		Cut muon_cuts = Cuts::abseta < 2.5 && Cuts::pT > 10*GeV;
		bool useDecayPhotons = false;

		// Dressed Leptons
		DressedLeptons dressedElectrons(photons, bare_el, 0.1, ele_cuts, useDecayPhotons);
		declare(dressedElectrons, "dressedElectrons");
		
		DressedLeptons dressedMuons(photons, bare_mu, 0.1, muon_cuts, useDecayPhotons);
		declare(dressedMuons, "dressedMuons");

		// Smearing Electrons
		SmearedParticles recoEle(dressedElectrons, ELECTRON_IDEFF_ATLAS_RUN2_LOOSE, ELECTRON_SMEAR_ATLAS_RUN1);
		recoEle.getLog().setLevel(print_level);
		declare(recoEle, "ElectronReco");
		MSG_DEBUG("recoEle: " << &recoEle);
		// Smearing Muons
		SmearedParticles recoMuons(dressedMuons, MUON_EFF_ATLAS_RUN1, MUON_SMEAR_ATLAS_RUN1);
		recoMuons.getLog().setLevel(print_level);
		declare(recoMuons, "MuonReco");
		MSG_DEBUG("recoMuons: " << &recoMuons);

		// Missing ET for all objects, including muons
		VisibleFinalState calofs(fs);
		MissingMomentum met(calofs);
		declare(met, "METTruth");
		met.getLog().setLevel(print_level);
		// Smearing MET
		SmearedMET recomet(met, MET_SMEAR_ATLAS_RUN1);
		recomet.getLog().setLevel(print_level);
		declare(recomet, "METReco");

		// Book Histograms for sanity checks
		_h_ptl 	   = bookHisto1D("lepton_pT", 100, 0, 200);
		_h_etal    = bookHisto1D("lepton_eta", 100, -5, 5);
		_h_mll     = bookHisto1D("mll", 400, 0, 200);
		_h_mlllog  = bookHisto1D("mll_log", 400, 1., 4.);
		_h_yll     = bookHisto1D("yll", 200, -5, 5);
		_h_ptll    = bookHisto1D("ptll", 200, 0, 400);
		_h_ptlllog = bookHisto1D("ptlllog", 200, -3, 3);
		_h_allHists.push_back(_h_ptl);
		_h_allHists.push_back(_h_etal);
		_h_allHists.push_back(_h_mll);
		_h_allHists.push_back(_h_mlllog);
		_h_allHists.push_back(_h_yll);
		_h_allHists.push_back(_h_ptll);
		_h_allHists.push_back(_h_ptlllog);
	}


	/// Perform the per-event analysis
	void analyze(const Event& event) 
	{
		MSG_DEBUG("Entering...");
		const double weight = event.weight();

		event.getLog().setLevel(Log::DEBUG);
		Cut jet_cuts = Cuts::pT > 30*GeV && Cuts::abseta < 2.8;
		const Jets truth_jets = apply<JetAlg>(event, "JetsTruth").jetsByPt(jet_cuts);
		const Jets reco_jets  = apply<JetAlg>(event, "JetsReco").jetsByPt(jet_cuts);
		MSG_DEBUG("Truth Jets:");
		for(const auto& j: truth_jets){
			MSG_DEBUG("\t(Eta, Phi, pT): "<< j.eta()<< ", " << j.phi() << ", " << j.pt());
		}
		MSG_DEBUG("Reco Jets:");
		for(const auto& j: reco_jets){
			MSG_DEBUG("\t(Eta, Phi, pT): "<< j.eta()<< ", " << j.phi()<< ", " << j.pt());
		}

		// electrons
		const Particles eleTruth = apply<IdentifiedFinalState>(event, "bareElectrons").particlesByPt();
		const vector<DressedLepton>& eleDressed = apply<DressedLeptons>(event, "dressedElectrons").dressedLeptons();
		const Particles eleReco  = apply<ParticleFinder>(event, "ElectronReco").particlesByPt();
		MSG_DEBUG("Bare Electrons: " << eleTruth.size() << ", Dressed Electrons: " << eleDressed.size() << ", Reco Electrons: " << eleReco.size());
		MSG_DEBUG("Bare Electrons: " << eleTruth.size());
		for(const auto& p: eleTruth) {
			MSG_DEBUG("\t(Eta, Phi, pT): "<< p.eta()<< ", " << p.phi()<< ", " << p.pt());
		}

		// muons
		const Particles muTruth  = apply<IdentifiedFinalState>(event, "bareMuons").particlesByPt();
		const vector<DressedLepton>& muonDressed = apply<DressedLeptons>(event, "dressedMuons").dressedLeptons();
		const Particles muReco   = apply<ParticleFinder>(event, "MuonReco").particlesByPt();
		MSG_DEBUG("Truth muons: " << muTruth.size() << ", Dressed Muons: " << muonDressed.size() << ", Reco muons: " << muReco.size());
		MSG_DEBUG("Bare Muons: " << muTruth.size());
		for(const auto& p: muTruth) {
			MSG_DEBUG("\t(Eta, Phi, pT): "<< p.eta()<< ", " << p.phi()<< ", " << p.pt());
		}


		// Event Selections:
		// 1. two muons with pT > 15 GeV and |eta| < 2.5; isolation cuts taken into efficiency?
		// 2. leading muons with pT > 27 GeV
		// 3.  no b-tagged jets and MET < 80 GeV
		// https://gitlab.cern.ch/lbnl/powheginlinegen/blob/master/apps/hmumu.cxx

		MSG_DEBUG("Leaving...");
	}

	/// Normalise histograms etc., after the run
	void finalize() {
		const double norm = 1.*crossSection()/femtobarn;
		for(auto& hist: _h_allHists) {
			scale(hist, norm/sumOfWeights());
		}
	}

private:
	Histo1DPtr _h_ptl;
	Histo1DPtr _h_etal;
	Histo1DPtr _h_mll;
	Histo1DPtr _h_mlllog;
	Histo1DPtr _h_yll;
	Histo1DPtr _h_ptll;
	Histo1DPtr _h_ptlllog;
	vector<Histo1DPtr> _h_allHists;
	bool use_truth;
};


// The hook for the plugin system
DECLARE_RIVET_PLUGIN(ATLAS_2017_I1599399);
}
