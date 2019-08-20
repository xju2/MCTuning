// -*- C++ -*-
#include "Rivet/Analysis.hh"
#include "Rivet/Projections/FinalState.hh"
#include "Rivet/Projections/IdentifiedFinalState.hh"
#include "Rivet/Projections/VisibleFinalState.hh"
#include "Rivet/Projections/FastJets.hh"
#include "Rivet/Tools/Logging.hh"

#include "HepMC/WeightContainer.h"

#include <stdio.h>
#include <algorithm>
#include <utility>
#include <ctime>
#include <functional>
#include <unistd.h> // For getpid()

// From ROOT
#include <TFile.h>
#include <TTree.h>
#include <TLorentzVector.h>
#include <TRandom.h>

// From PowhegInlineGen
#include "NtupleDumper.h"

#include "EgammaSmear.h"
#include "MuonSmearGiacomoPU.h"
#include "JetSmear.h"
#include "MyJet.h"

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
		gRandom->SetSeed();
		// this->getLog().setLevel(Log::INFO);
		// int print_level = Log::DEBUG;
		// int print_level = Log::INFO;

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
		declare(jets, "JetsTruth");

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
		muonSmear2 = new MuonSmearGiacomoPU(true);
		mu_sys_names = muonSmear2->GetMuonSysNames();
		egammaSmear = new EgammaSmear();
		jetSmear = new JetSmear();
		if(doPileupJets) {
			jetSmear->InitPileupJets();
		}
		if(smearMET) {
			char buffer[512];
			sprintf(buffer, "%s/smear_metparam_v16.root", getenv("ATLAS_SMEAR_FILES_DIR"));
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
		// std::time_t now = std::time(nullptr);
		// int file_seed = (int) now + (int)gRandom->Rndm()*10; 
		int file_seed = getpid();
		sprintf(buffer, "HmumuNtuple_%d.root", file_seed);
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
		// ntuple->Print();

		n_evt_3muons = 0;
		nEvents = 0;
		sumWgt = 0;
		sumWgt2 = 0;
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

	int RunNumberToMCPeriod(unsigned int random) {
		int period = -1;
		if (random < 320000) {
			period = 0;
		} else if (random < 342000) {
			period = 1;
		} else {
			period = 2;
		}
		return period;
	}


	/// Perform the per-event analysis
	void analyze(const Event& event) 
	{
		MSG_DEBUG("Entering...");
		const double weight = event.weight();
		float det_weight = 1.0;

		nEvents ++;
		sumWgt += weight;
		sumWgt2 += weight*weight;
		TLorentzVector tlv_zero(0., 0., 0., 0.);

		// Truth studies
		// 
		// Truth muons
		const Particles muTruth  = apply<IdentifiedFinalState>(event, "bareMuons").particlesByPt();

		// Truth Jets
		// remove jets with low track multiplicity and close to muons (DR < 0.4)
		Cut jet_cuts = Cuts::pT > 20*GeV && Cuts::abseta < 4.5;
		const Jets truth_jets = apply<JetAlg>(event, "JetsTruth").jetsByPt(jet_cuts);
		const Jets isojets = filter_discard(truth_jets, [&](const Jet& j) {
				if(j.particles(Cuts::abscharge > 0 && Cuts::pT > 0.4*GeV).size() <= 3
					&& (any(muTruth, deltaRLess(j, 0.4)))) return true;
				return false;
				});
		// remove muons that are close to surviving jets (DR < 0.4)
		const Particles iso_muons = filter_discard(muTruth, [&](const Particle& m) {
			for(const Jet& j: isojets) {
				if(deltaR(j, m) > 0.4) continue;
				if(j.particles(Cuts::abscharge > 0 && Cuts::pT > 0.4*GeV).size() > 3) return true;
			}
			return false;
			});

		if(iso_muons.size() != 2
			|| iso_muons.at(0).pid() * iso_muons.at(1).pid() > 0) {
			return;
		}


		int l1_idx_gen = 0, l2_idx_gen = 1;

		TLorentzVector vecl1_rad = to_tlv(iso_muons[l1_idx_gen].momentum());
		TLorentzVector vecl2_rad = to_tlv(iso_muons[l2_idx_gen].momentum());
		int idl1 = iso_muons[l1_idx_gen].pid();

		// Truth Photons
		const Particles phTruth = apply<IdentifiedFinalState>(event, "photonTruth").particlesByPt();
		const Particles iso_photons = filter_discard(phTruth, [&](const Particle& p) {
			return p.fromDecay();
			});


		TLorentzVector leading_photon_gen;
		if(iso_photons.size() > 0) {
			leading_photon_gen = to_tlv(iso_photons[0].momentum());
		} else {
			leading_photon_gen = tlv_zero;
		}

		// MyJet saves the pid for the jet
		// which is used in JetSmearing
		vector<MyJet> jets_gen;
		vector<TLorentzVector> jets_gen_subthreshold;
		for(auto& jet: isojets) {
			FourMomentum momentum = jet.momentum();
			TLorentzVector jet_tlv = to_tlv(momentum);

			if(momentum.pt()/GeV < minGenJetPt
					|| momentum.abseta() > maxGenJetRapidity){
				jets_gen_subthreshold.push_back(to_tlv(momentum));
			} else {
				jets_gen.push_back( MyJet(to_tlv(momentum), 21) );
			}
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
		for(auto& photon: iso_photons) {
			TLorentzVector ph_tlv = to_tlv(photon.momentum());
			photons_gen.push_back(ph_tlv);
			hr_gen += ph_tlv;
			sumet_gen += photon.pt();
		}
		for(auto& muon: iso_muons) {
			FourMomentum momentum = muon.momentum();
			hr_gen += to_tlv(muon.momentum());
		}
		TLorentzVector met_gen = -1*hr_gen;

		TLorentzVector vecB = vecl1_rad + vecl2_rad;
		ntuple->SetTruthLevel(weight,
				(idl1>0)?vecl1_rad:vecl2_rad,
				(idl1>0)?vecl2_rad:vecl1_rad,
				leading_photon_gen,
				jets_gen,
				vecB.M());
		// DONE with Truth Studies

		// Smearing Objects,
		// First Get PileupJets, to determine the MC period

		// Jets smeared differently for gluon-jet and quark-jet
		// only smeare the jets that pass losse truth cuts: pT > 20 GeV.
		vector<TLorentzVector> jets_det;
		vector<TLorentzVector> jets_det_subthreshold;
		for(auto jet: isojets)
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
		unsigned int randomRunNumber = 320032; // mc16d
		float pileupWeight = 1.0;
		if(doPileupJets) {
			jetSmear->ReadPileupJets(nEvents);
			std::vector<TLorentzVector> puj = jetSmear->GetPileupJets();
			for (unsigned int i = 0; i < puj.size(); i++) {
				puj[i] *= 1e-3; // MeV -> GeV
				if (isSubThresholdDetJet(puj.at(i)))
					jets_det_subthreshold.push_back(puj.at(i));
				else
					jets_det.push_back(puj.at(i));
			}
			randomRunNumber = jetSmear->GetRandomRunNumber();
			pileupWeight = jetSmear->GetPileupWeight();
		}

		MSG_DEBUG("Pileup...sort");
		sort(jets_det.begin(), jets_det.end(),
			[](const TLorentzVector& a, const TLorentzVector& b){ return a.Pt() > b.Pt(); }
			);
		int mc_period = RunNumberToMCPeriod(randomRunNumber);


		// use bare muons,
		vecl1_det = muonSmear2->GetSmearedMuon(vecl1_rad, mc_period);
		l1_ptreso = muonSmear2->GetLastPtReso();
		mu1_sys_ptfacts = muonSmear2->GetMuonSysPtfacts();

		vecl2_det = muonSmear2->GetSmearedMuon(vecl2_rad, mc_period);
		l2_ptreso = muonSmear2->GetLastPtReso();
		mu2_sys_ptfacts = muonSmear2->GetMuonSysPtfacts();

		double muon_eff = muonSmear2->GetDiMuonEff(vecl1_rad, vecl2_rad);
		// if(abs(muon_eff) < 1e-5) {
		// 	MSG_INFO("Leading pT: " << vecl1_rad.Pt() << " " << vecl1_rad.Eta() << " ");
		// 	MSG_INFO("SubLeading pT: " << vecl2_rad.Pt() << " " << vecl2_rad.Eta() << " ");
		// }
		det_weight *= muon_eff;

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
		for(auto& ph : photons_det) {
			hr_det += ph;
		}
		TLorentzVector dilep = vecl1_det + vecl2_det;
		double ptll = dilep.Pt();
		if(smearMET) {
			// smearing was done for 0, 1, and 2jets events.
			int cat = 0;
			if (jets_det.size() == 1)
				cat = 1;
			else if (jets_det.size() >= 2)
				cat = 2;
			int bin = h_metSmear[cat*3]->FindBin(ptll);
			double bias_par = h_metSmear[cat*3+0]->GetBinContent(bin);
			double resol_par = h_metSmear[cat*3+1]->GetBinContent(bin);
			double resol_perp = h_metSmear[cat*3+2]->GetBinContent(bin);

			upar_det  = (hr_det.Px()*dilep.Px() + hr_det.Py()*dilep.Py())/ptll;
			uperp_det = (hr_det.Py()*dilep.Px() - hr_det.Px()*dilep.Py())/ptll;

			double smeared_HR_par  = gRandom->Gaus(upar_det+bias_par, resol_par);
			double smeared_HR_perp = gRandom->Gaus(uperp_det, resol_perp);

			double cosphi = cos(dilep.Phi());
			double sinphi = sin(dilep.Phi());
			double smeared_HR_Px = smeared_HR_par*cosphi-smeared_HR_perp*sinphi;
			double smeared_HR_Py = smeared_HR_par*sinphi+smeared_HR_perp*cosphi;
			double smeared_HR_E = sqrt(smeared_HR_par*smeared_HR_par
					+smeared_HR_perp*smeared_HR_perp);
			hr_det.SetPxPyPzE(smeared_HR_Px, smeared_HR_Py, 0., smeared_HR_E);
		}
		upar_det  = (hr_det.Px()*dilep.Px() + hr_det.Py()*dilep.Py())/ptll;
		uperp_det = (hr_det.Py()*dilep.Px() - hr_det.Px()*dilep.Py())/ptll;

		met_det = -( hr_det + dilep );

		MSG_DEBUG("RecoLevel...");
		// Event Selections:
		// 1. two muons with pT > 15 GeV and |eta| < 2.5; isolation cuts taken into efficiency?
		// 2. leading muons with pT > 27 GeV
		// 3.  no b-tagged jets 
		// https://gitlab.cern.ch/lbnl/powheginlinegen/blob/master/apps/hmumu.cxx
		ntuple->SetRecoLevel(det_weight,
				(idl1>0)?vecl1_det:vecl2_det,
				(idl1>0)?vecl2_det:vecl1_det,
				leading_photon_det, // This is FSR photon!!!
				jets_det,
				met_det, 
				l1_ptreso, l2_ptreso,
				mu1_sys_ptfacts, mu2_sys_ptfacts,
				pileupWeight
				);

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
	MuonSmearGiacomoPU *muonSmear2;
	EgammaSmear *egammaSmear;
	JetSmear *jetSmear;
	TH1 *h_metSmear[3*3];

	NtupleDumper* ntuple;
	int n_evt_3muons;
};


// The hook for the plugin system
DECLARE_RIVET_PLUGIN(ATLAS_2017_I1599399);
}
