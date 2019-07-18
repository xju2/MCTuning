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

#include "HepMC/WeightContainer.h"

#include <stdio.h>
#include <algorithm>
#include <utility>

// From ROOT
#include <TFile.h>
#include <TTree.h>
#include <TLorentzVector.h>
#include <TRandom.h>

// From PowhegInlineGen
#include "NtupleDumper.h"

#include "EgammaSmear.h"
#include "MuonSmearGiacomo.h"
#include "JetSmear.h"

namespace Rivet {

  /// @brief Add a short analysis description here
class ATLAS_2017_I1599399 : public Analysis {
public:

    /// Constructor
    DEFAULT_RIVET_ANALYSIS_CTOR(ATLAS_2017_I1599399);
	// TODO: 
	// 1. multi-weights [Not to add for now, seems not sensible to the analysis]
	// 2. MET [Done, need validation]
	// 3. muon systematic uncertainties
	// 4. Pileup jets [done]
	// 5. should smear bare muons, not dressed muons!!!! [done]
	// 
	// Questions:
	// 1. why Gen jet has different cuts?? [Loose on Truth]
	// 2. MET includes muons, but sumET does not?
	// 3. met_gen calculated, but not used
	// 4. MET smearing was done for 0, 1 and 2 jets. 
	//    Now we want to generate up to 3 jets


    /// Book histograms and initialise projections before the run
	void init() {
		// this->getLog().setLevel(Log::INFO);
		// int print_level = Log::DEBUG;
		// int print_level = Log::INFO;
		int print_level = Log::INFO;

		// this->getLog().setLevel(Log::DEBUG);

		doJVTEff = true;
		doPileupJets = true;
		smearMET = true;

		minDetJetPtCen = 25.;
		minDetJetPtFwd = 30.;
		minGenJetPt = 20.;
		maxGenJetRapidity = 5.;
		double maxEtaPhoton = 2.5, minPtPhoton = 5.;

		// Jets
		FinalState fs(Cuts::abseta < 4.5);
		FastJets jets(fs, FastJets::ANTIKT, 0.4);
		declare(eets, "JetsTruth");

		// photons
		IdentifiedFinalState photons(Cuts::abseta < maxEtaPhoton && Cuts::pT > minPtPhoton);
		photons.acceptIdPair(PID::PHOTON);
		declare(photons, "photonTruth");

		// Bare Muons, very loose cuts
		Cut muon_cuts = Cuts::abseta < 2.7 && Cuts::pT > 10*GeV; 
		IdentifiedFinalState bare_mu(muon_cuts);
		bare_mu.acceptIdPair(PID::MUON);
		declare(bare_mu, "bareMuons");


		// Smearing
		int seed = (int)rand01()*300; //[TODO]
		muonSmear2 = new MuonSmearGiacomo(seed%3, true);
		mu_sys_names = muonSmear2->GetMuonSysNames();
		egammaSmear = new EgammaSmear();
		jetSmear = new JetSmear();
		if(doPileupJets) {
			jetSmear->InitPileupJets();
		}
		if(smearMET) {
			char buffer[512];
			int n_char; 
			n_char = sprintf(buffer, "%s/smear_metparam.root", getenv("ATLAS_SMEAR_FILES_DIR"));
			TFile* f = TFile::Open((const char*)buffer);
			const std::vector<std::string> cats{"0jet", "1jet", "2jet"};
			int i = 0;
			for (auto & cat : cats) {
				h_metSmear[i++] = (TH1*) f->Get(TString::Format("corr_%s/uparpt_1", cat.c_str()));
				h_metSmear[i++] = (TH1*) f->Get(TString::Format("corr_%s/uparpt_2", cat.c_str()));
				h_metSmear[i++] = (TH1*) f->Get(TString::Format("corr_%s/uperppt_2", cat.c_str()));
			}
			for (i = 0; i < 9; i++)
				h_metSmear[i]->SetDirectory(0);
			f->Close();
		}

		//Prepare for Ntuple
		ntheoweights = 4;
		char buffer[512];
		sprintf(buffer, "HmumuNtuple_%d.root", seed);
		string filename(buffer);
		bool doTruth = true;
		bool doReco = true;
		double maxEtaL = 2.7, minPtL = 15.;
		double maxEtaJet = 4.5, minPtJet = 25;
		double lowMllBoundary= 0, lowMllPrescale= 1.;
		maxMllNtuple = 1e5;
		
		ntuple = new NtupleDumper(filename, doTruth, doReco, ntheoweights,
								  mu_sys_names);
		
		ntuple->SetMaxLeptonEta(maxEtaL);
		ntuple->SetMinLeptonPt(minPtL);
		ntuple->SetMaxJetEta(maxEtaJet);
		ntuple->SetMinJetPt(minPtJet);
		ntuple->SetMaxPhotonEta(maxEtaPhoton);
		ntuple->SetMinPhotonPt(minPtPhoton);
		ntuple->SetMassPrescale(lowMllBoundary, lowMllPrescale);
		ntuple->Print();

		n_evt_3muons = 0;

	}

	// if jet passed pT selection
	bool isSubThresholdDetJet(const TLorentzVector & jet) const
	{
		double absjeta = TMath::Abs(jet.Eta());
		if (absjeta < 2.5 && jet.Pt() >= minDetJetPtCen)
			return false;
		if (2.5 <= absjeta && absjeta < 4.5 && jet.Pt() >= minDetJetPtFwd)
			return false;
		return true;
	}

	
	TLorentzVector to_tlv(const FourMomentum& momentum) {
		TLorentzVector inParticle;
		inParticle.SetPxPyPzE(
				momentum.px()/GeV,
				momentum.py()/GeV,
				momentum.pz()/GeV, 
				momentum.E()/GeV
				);
		return inParticle;
	}


	/// Perform the per-event analysis
	void analyze(const Event& event) 
	{
		MSG_DEBUG("Entering...");
		const double weight = event.weight();

		nEvents ++;
		sumWgt += weight;
		sumWgt2 += weight*weight;
		TLorentzVector tlv_zero(0., 0., 0., 0.);

		// Truth studies
		// 
		// Truth muons
		const Particles muTruth  = apply<IdentifiedFinalState>(event, "bareMuons").particlesByPt();

		int l1_idx_gen = -1, l2_idx_gen = -1;
		bool found = false;

		if(muTruth.size() == 2
		   && muTruth.at(0).pid() * muTruth.at(1).pid() < 0) {
			l1_idx_gen = 0; l2_idx_gen = 1;
			found = true;
		} else if (muTruth.size() > 2) {
			n_evt_3muons ++ ;
			for(int i=0; i < (int) muTruth.size(); i++) {
				int pid1 = muTruth.at(i).pid();
				for(int j=i+1; j < (int) muTruth.size(); j++) {
					int pid2 = muTruth.at(j).pid();
					if(pid1*pid2 < 0) {
						l1_idx_gen = i;
						l2_idx_gen = j;
						found = true;
						break;
					}
				}
				if(found) break;
			}
		} else {
			return;
		}

		if(!found) return;

		TLorentzVector vecl1_rad = to_tlv(muTruth[l1_idx_gen].momentum());
		TLorentzVector vecl2_rad = to_tlv(muTruth[l2_idx_gen].momentum());
		int idl1 = muTruth[l1_idx_gen].pid();

		// Truth Photons
		const Particles phTruth = apply<IdentifiedFinalState>(event, "photonTruth").particlesByPt();
		TLorentzVector leading_photon_gen;
		if(phTruth.size() > 0) {
			leading_photon_gen = to_tlv(phTruth[0].momentum());
		} else {
			leading_photon_gen = tlv_zero;
		}

		// Truth Jets
		Cut jet_cuts = Cuts::pT > 20*GeV && Cuts::abseta < 4.5;
		const Jets truth_jets = apply<JetAlg>(event, "JetsTruth").jetsByPt(jet_cuts);
		vector<TLorentzVector> jets_gen;
		vector<TLorentzVector> jets_gen_subthreshold;
		for(auto& jet: truth_jets) {
			FourMomentum momentum = jet.momentum();
			TLorentzVector jet_tlv = to_tlv(momentum);

//		if(isSubThresholdDetJet(jet_tlv)){
//			jets_gen_subthreshold.push_back(jet_tlv);
//		} else {
//			jets_gen.push_back(jet_tlv);
//		}

			if(momentum.pt()/GeV < minGenJetPt
					|| momentum.abseta() > maxGenJetRapidity){
				jets_gen_subthreshold.push_back(to_tlv(momentum));
			} else {
				jets_gen.push_back(to_tlv(momentum));
			}
		}
		// Find leading two jets
		TLorentzVector leading_jet_gen;
		TLorentzVector subleading_jet_gen;
		if(jets_gen.size() == 0){
			leading_jet_gen = tlv_zero;
		} else {
			leading_jet_gen = jets_gen.at(0);
		}
		if(jets_gen.size() <= 1){
			subleading_jet_gen = tlv_zero;
		} else {
			subleading_jet_gen = jets_gen.at(1);
		}

		// Reconstruct MET, jets-photons-muons
		double sumet_gen = 0;
		TLorentzVector hr_gen;
		hr_gen.SetXYZT(0., 0., 0., 0.);
		for(auto& jet: jets_gen) {
			hr_gen += jet;
			sumet_gen += jet.Pt();
		}
		for(auto& jet: jets_gen_subthreshold) {
			hr_gen += jet;
			sumet_gen += jet.Pt();
		}

		vector<TLorentzVector> photons_gen;
		for(auto& photon: phTruth) {
			TLorentzVector ph_tlv = to_tlv(photon.momentum());
			photons_gen.push_back(ph_tlv);
			hr_gen += ph_tlv;
			sumet_gen += photon.pt();
		}
		for(auto& muon: muTruth) {
			FourMomentum momentum = muon.momentum();
			hr_gen += to_tlv(muon.momentum());
		}
		TLorentzVector met_gen = -1*hr_gen;

		TLorentzVector vecB = vecl1_rad + vecl2_rad;
		ntuple->SetTruthLevel(weight,
				(idl1>0)?vecl1_rad:vecl2_rad,
				(idl1>0)?vecl2_rad:vecl1_rad,
				leading_photon_gen,
				leading_jet_gen,
				subleading_jet_gen,
				vecB.M());
		// DONE with Truth Studies
		// 

		// Smearing Objects,
		// use bare muons,
		vector<pair<TLorentzVector, int> > smeared_muons;
		vector<int> muons_pid;
		vector<float> ptreso_vec;
		vector<vector<double> > mu_sys_ptfacts;
		int idx = 0;
		for(auto muon: muTruth){
			TLorentzVector smeared_muon = muonSmear2->GetSmearedMuon(to_tlv(muon.momentum()));
			smeared_muons.push_back(make_pair(smeared_muon, idx));

			muons_pid.push_back(muon.pid());
			ptreso_vec.push_back(muonSmear2->GetLastPtReso());
			mu_sys_ptfacts.push_back(muonSmear2->GetMuonSysPtfacts());
			idx ++;
		}

		// sort semared muons
		sort(smeared_muons.begin(), smeared_muons.end(),
				[](pair<TLorentzVector, int>& a, pair<TLorentzVector, int>& b){ return a.first.Pt() > b.first.Pt(); }
			);

		//Find index of the leading two oppositely-charged muons
		found = false;
		int l1_idx = -1, l2_idx = -1;
		if(smeared_muons.size() == 2) {
			l1_idx = 0;
			l2_idx = 1;
			found = true;
		} else {
			for(int i = 0; i < (int) smeared_muons.size(); i++){
				int idx1 = smeared_muons.at(i).second;
				int pid1 = muons_pid.at(idx1);
				for(int j = i+1; j < (int) smeared_muons.size(); j++) {
					int idx2 = smeared_muons.at(j).second;
					int pid2 = muons_pid.at(idx2);
					if(pid1 * pid2 < 0) {
						l1_idx = i;
						l2_idx = j;
						found = true;
						break;
					}
				}
				if(found) break;
			}
		}

		if(l1_idx < 0 || l2_idx < 0) 
			return ;


		vecl1_det = smeared_muons.at(l1_idx).first;
		vecl2_det = smeared_muons.at(l2_idx).first;
		int idx1  = smeared_muons.at(l1_idx).second;
		int idx2  = smeared_muons.at(l2_idx).second;

		l1_ptreso = ptreso_vec.at(idx1);
		l2_ptreso = ptreso_vec.at(idx2);
		mu1_sys_ptfacts = mu_sys_ptfacts.at(idx1);
		mu2_sys_ptfacts = mu_sys_ptfacts.at(idx2);


		// Jets smeared differently for gluon-jet and quark-jet
		// only smeare the jets that pass losse truth cuts: pT > 20 GeV.
		vector<TLorentzVector> jets_det;
		vector<TLorentzVector> jets_det_subthreshold;
		for(auto jet: truth_jets)
		{
			if(jet.pt()/GeV < minGenJetPt || jet.abseta() > maxGenJetRapidity) 
				continue;

			// Is the jet from quark or gluon?
			// If constitunants are mostly jets, then jets
			// After discussing with Jan, treat jets from gluons
			//int n_quarks = 0, n_gluons = 0, n_nothing = 0;
			//for(auto& pp: jet.particles()) {
			//	int pid = pp.pid();
			//	if(PID::isQuark(pp.pid())) n_quarks ++;
			//	else if(PID::isGluon(pp.pid())) n_gluons ++;
			//	else n_nothing ++;
			//	MSG_DEBUG("jet particles: "  <<  pid);
			//}
			//int pid = n_quarks>n_gluons?1:21;
			//MSG_DEBUG("jet: nquark "  <<  n_quarks << " ngluon: " << n_gluons << " nnothing: " << n_nothing);

			int pid = 21;
			FourMomentum momentum = jet.momentum();
			TLorentzVector sjet = jetSmear->GetSmearedJet(to_tlv(momentum), pid);
			if (isSubThresholdDetJet(sjet)
				|| (doJVTEff && gRandom->Rndm() > jetSmear->GetJVTEff(sjet))) {
				jets_det_subthreshold.push_back(sjet);
			} else {
				jets_det.push_back(sjet);
			}
		}

		MSG_DEBUG("Pileup...");
		// generously smear+suppress the sub-threshold "jets"
		for (unsigned int i = 0; i < jets_gen_subthreshold.size(); i++) {
			double smearfac = TMath::Max(0., gRandom->Gaus(0.6, 0.3));
			TLorentzVector sjet = jets_gen_subthreshold[i]*smearfac;
			jets_det_subthreshold.push_back(sjet);
		}
		MSG_DEBUG("Pileup...start");
		if(doPileupJets) {
			std::vector<TLorentzVector> puj = jetSmear->GetPileupJets(nEvents);
			MSG_DEBUG("Pileup...start-1");
			for (unsigned int i = 0; i < puj.size(); i++) {
				puj[i] *= 1e-3; // MeV -> GeV
				if (isSubThresholdDetJet(puj.at(i)))
					jets_det_subthreshold.push_back(puj.at(i));
				else
					jets_det.push_back(puj.at(i));
			}
		}

		MSG_DEBUG("Pileup...sort");

		sort(jets_det.begin(), jets_det.end(),
			[](const TLorentzVector& a, const TLorentzVector& b){ return a.Pt() > b.Pt(); }
			);

		MSG_DEBUG("Photons...");
		// Photon smearing
		vector<TLorentzVector> photons_det;
		for(auto& photon : photons_gen) {
			if(photon.Pt() < 1) continue;

			double eff = egammaSmear->GetFSRPhotonEff(
					photon, vecl1_rad, vecl2_rad);
			if (gRandom->Rndm() > eff) continue;

			TLorentzVector sphoton = egammaSmear->GetSmearedPhoton(photon);

			// remove photons of smeared pT less than 5 GeV
			if (sphoton.Pt() < 5) continue;

			photons_det.push_back(sphoton);
		}
		sort(photons_det.begin(), photons_det.end(),
				[](const TLorentzVector& a, const TLorentzVector& b){ return a.Pt() > b.Pt(); }
			);

		TLorentzVector leading_photon_det;
		if(photons_det.size() < 1 ){
			leading_photon_det = tlv_zero;
		} else {
			leading_photon_det = photons_det[0];
		}

		MSG_DEBUG("MET...");
		// Reconstruct MET in detector level
		hr_det.SetXYZT(0., 0., 0., 0.);
		sumet_det = 0;

		for(auto& jet : jets_det) {
			hr_det += jet;
			sumet_det += jet.Pt();
		}
		for(auto& jet : jets_det_subthreshold) {
			hr_det += jet;
			sumet_det += jet.Pt();
		}
		TLorentzVector dilep = vecl1_det + vecl2_det;
		double ptll = dilep.Pt();
		if(smearMET) {
			// smearing was done for 0, 1, and 2jets events.
			// how about now??
		}
		met_det = -( hr_det + dilep );

		MSG_DEBUG("RecoLevel...");
		// Event Selections:
		// 1. two muons with pT > 15 GeV and |eta| < 2.5; isolation cuts taken into efficiency?
		// 2. leading muons with pT > 27 GeV
		// 3.  no b-tagged jets 
		// https://gitlab.cern.ch/lbnl/powheginlinegen/blob/master/apps/hmumu.cxx
		ntuple->SetRecoLevel(weight,
				(idl1>0)?vecl1_det:vecl2_det,
				(idl1>0)?vecl2_det:vecl1_det,
				leading_photon_det, // This is FSR photon!!!
				jets_det,
				met_det, 
				l1_ptreso, l2_ptreso,
				mu1_sys_ptfacts, mu2_sys_ptfacts);

		if (vecB.M()/GeV < maxMllNtuple){
			ntuple->Fill();
		}

		MSG_DEBUG("Leaving...");
	}

	/// Normalise histograms etc., after the run
	void finalize() {
		// double xsection = crossSection()/femtobarn;
		double xsection = crossSection()/picobarn;
		double luminosity = sumWgt/xsection;
		double effective_lumi = sumWgt*sumWgt/sumWgt2/xsection;

		/// const double norm = 1.*crossSection()/femtobarn;
		TH1* hweights = NULL; // TODO add weights...
		ntuple->Finalise(luminosity, xsection, sumWgt, effective_lumi, hweights);
		delete muonSmear2;
		delete egammaSmear;
		delete jetSmear;
		delete ntuple;
		for (int i = 0; i < 9; i++) {
			if(h_metSmear[i]) delete h_metSmear[i];
		}
		MSG_INFO("Events with more than 2 muons: " << n_evt_3muons);
	}


private:
	bool doJVTEff;
	bool doPileupJets;
	bool smearMET;
	float minDetJetPtCen;
  	float minDetJetPtFwd;
	float minGenJetPt;
	float maxGenJetRapidity;
	float dressCone;

	// muon systematics
	std::vector<std::string> mu_sys_names;
	std::vector<double> mu1_sys_ptfacts;
	std::vector<double> mu2_sys_ptfacts;

	// the detector-level-like kinematics & weight of the current event
	TLorentzVector vecl1_det;
	TLorentzVector vecl2_det;
	float l1_ptreso;
	float l2_ptreso;

	TLorentzVector hr_det;
	TLorentzVector met_det;
	double sumet_det;
	double upar_det;
	double uperp_det;

	double genweight;
	double lastgenweight;
	double detweight;

	// statistics
	double genCrossSection;
	long int nEvents;
	long int printInterval;
	bool printIntervalLogIncrease;
	double sumWgt;
	double sumWgt2;

	double maxMllNtuple;

	int ntheoweights;
	MuonSmearGiacomo *muonSmear2;
	EgammaSmear *egammaSmear;
	JetSmear *jetSmear;
	TH1 *h_metSmear[3*3];

	NtupleDumper* ntuple;
	int n_evt_3muons;
};


// The hook for the plugin system
DECLARE_RIVET_PLUGIN(ATLAS_2017_I1599399);
}
