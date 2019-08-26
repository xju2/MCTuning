#include <TFile.h>
#include <TVector2.h>
#include <TLorentzVector.h>
#include <TH1D.h>
#include <TTree.h>
#include <vector>

using namespace std;
int flatten(const char* in_file_name, const char* out_file_name, bool reweight=true)
{
	TFile* in_file = TFile::Open(in_file_name, "READ");
	const char* tree_name = "DiMuonNtuple";
	TTree* oldtree = (TTree*) in_file->Get(tree_name);
	oldtree->SetBranchStatus("ClassOut_XGB_fJVT_Higgs", 0);
	oldtree->SetBranchStatus("ClassOut_XGB_fJVT_VBF", 0);
	oldtree->SetBranchStatus("Event_XGB_fJVT_Category", 0);

	Long64_t nentries = oldtree->GetEntries();
	TH1D* lumi = (TH1D*) in_file->Get("lumi");
	if(lumi) {
		lumi->SetDirectory(0);
	}

	vector<float>* jets_pt = new vector<float>();
	vector<float>* jets_eta = new vector<float>();
	vector<float>* jets_phi = new vector<float>();
	float mcweight;
	float expweight;
	float muon_lead_eta, muon_lead_phi, muon_lead_pt;
	float muon_sub_eta, muon_sub_phi, muon_sub_pt;
	float fsr_eta, fsr_phi, fsr_pt;
	oldtree->SetBranchAddress("Jets_PT", &jets_pt);
	oldtree->SetBranchAddress("Jets_Eta", &jets_eta);
	oldtree->SetBranchAddress("Jets_Phi", &jets_phi);
	oldtree->SetBranchAddress("McEventWeight", &mcweight);
	oldtree->SetBranchAddress("ExpWeight", &expweight);

	oldtree->SetBranchAddress("Muons_Eta_Lead", &muon_lead_eta);
	oldtree->SetBranchAddress("Muons_Phi_Lead", &muon_lead_phi);
	oldtree->SetBranchAddress("Muons_PT_Lead", &muon_lead_pt);
	oldtree->SetBranchAddress("Muons_Eta_Sub", &muon_sub_eta);
	oldtree->SetBranchAddress("Muons_Phi_Sub", &muon_sub_phi);
	oldtree->SetBranchAddress("Muons_PT_Sub", &muon_sub_pt);
	oldtree->SetBranchAddress("FSR_Eta", &fsr_eta);
	oldtree->SetBranchAddress("FSR_Phi", &fsr_phi);
	oldtree->SetBranchAddress("FSR_Et", &fsr_pt);

	TFile* out_file = TFile::Open(out_file_name, "RECREATE");
	TTree* newtree = oldtree->CloneTree(0);
	if (lumi){
		lumi->Write();
	}
	double luminosity = lumi->GetBinContent(1);
	// default 30 MB, now 300 MB
	newtree->SetAutoFlush(300000000);
	
	// additional variables
	float weight;
	float leading_jet_pT, leading_jet_eta, leading_jet_phi, leading_jet_E;
	float sub_jet_pT, sub_jet_eta, sub_jet_phi, sub_jet_E;
	int njets = 0;
	float dphi_lead_jetZ, dphi_sub_jetZ;
	float dijet_pt, dijet_rapidity, dijet_dphi, dijet_Minv;
	float dimuon_pt, dimuon_rapidity;
	newtree->Branch("Jets_PT_Lead", &leading_jet_pT, "Jets_PT_Lead/F");
	newtree->Branch("Jets_Eta_Lead", &leading_jet_eta, "Jets_Eta_Lead/F");
	newtree->Branch("Jets_Phi_Lead", &leading_jet_phi, "Jets_Phi_Lead/F");
	newtree->Branch("Jets_E_Lead", &leading_jet_E, "Jets_E_Lead/F");
	newtree->Branch("Dphi_JZ_Lead", &dphi_lead_jetZ, "Dphi_JZ_Lead/F");

	newtree->Branch("Jets_PT_Sub", &sub_jet_pT, "Jets_PT_Sub/F");
	newtree->Branch("Jets_Eta_Sub", &sub_jet_eta, "Jets_Eta_Sub/F");
	newtree->Branch("Jets_Phi_Sub", &sub_jet_phi, "Jets_Phi_Sub/F");
	newtree->Branch("Jets_E_Sub", &sub_jet_E, "Jets_E_Sub/F");
	newtree->Branch("Dphi_JZ_Sub", &dphi_sub_jetZ, "Dphi_JZ_Sub/F");

	newtree->Branch("DiJet_PT", &dijet_pt, "DiJet_PT/F");
	newtree->Branch("DiJet_Rapidity", &dijet_rapidity, "DiJet_Rapidity/F");
	newtree->Branch("DiJet_Dphi", &dijet_dphi, "DiJet_Dphi/F");
	newtree->Branch("DiJet_Minv", &dijet_Minv, "DiJet_Minv/F");

	newtree->Branch("Z_PT", &dimuon_pt, "Z_PT/F");
	newtree->Branch("Z_Rapidity", &dimuon_rapidity, "Z_Rapidity/F");

	newtree->Branch("weight", &weight, "weight/F");
	newtree->Branch("Jets_jetMultip", &njets, "Jets_jetMultip/I");
	
	for(Long64_t i=0; i < nentries; i++) {
		oldtree->GetEntry(i);
		njets = jets_pt->size();
		leading_jet_pT  = -9999;
		leading_jet_eta = -9999;
		leading_jet_phi = -9999;
		leading_jet_E = -9999;
		dphi_lead_jetZ = -9999;

		sub_jet_pT  = -9999;
		sub_jet_eta = -9999;
		sub_jet_phi = -9999;
		sub_jet_E = -9999;
		dphi_sub_jetZ = -9999;

		dijet_pt = -9999;
		dijet_rapidity = -9999;
		dijet_dphi = -9999;
		dijet_Minv = -9999;

		// Variables for BDT
		TLorentzVector Muons_Lead;
		Muons_Lead.SetPtEtaPhiM(muon_lead_pt, muon_lead_eta, muon_lead_phi, 0.106);
		TLorentzVector Muons_Sub;
		Muons_Sub.SetPtEtaPhiM(muon_sub_pt, muon_sub_eta, muon_sub_phi, 0.106);
		TLorentzVector FSR;
		FSR.SetPtEtaPhiM(fsr_pt, fsr_eta, fsr_phi, 0);

		TLorentzVector diMuon = Muons_Lead + Muons_Sub + FSR;
		dimuon_pt = diMuon.Pt();
		dimuon_rapidity = diMuon.Rapidity();

		if(jets_pt->size() > 0) {
			leading_jet_pT = jets_pt->at(0);
			leading_jet_eta = jets_eta->at(0);
			leading_jet_phi = jets_phi->at(0);
			leading_jet_E = leading_jet_pT * TMath::CosH(leading_jet_eta);
			dphi_lead_jetZ = TVector2::Phi_mpi_pi(leading_jet_phi-diMuon.Phi());
		}

		if(jets_pt->size() > 1) {
			sub_jet_pT = jets_pt->at(1);
			sub_jet_eta = jets_eta->at(1);
			sub_jet_phi = jets_phi->at(1);
			sub_jet_E = sub_jet_pT * TMath::CosH(sub_jet_eta);
			dphi_sub_jetZ = TVector2::Phi_mpi_pi(sub_jet_phi-diMuon.Phi());
			TLorentzVector J1;
			TLorentzVector J2;
			J1.SetPtEtaPhiE(leading_jet_pT, leading_jet_eta, leading_jet_phi, leading_jet_E);
			J2.SetPtEtaPhiE(sub_jet_pT, sub_jet_eta, sub_jet_phi, sub_jet_E);
			TLorentzVector DiJet = J1+J2;
			dijet_pt = DiJet.Pt();
			dijet_rapidity = DiJet.Rapidity();
			dijet_dphi = TVector2::Phi_mpi_pi(DiJet.Phi() - diMuon.Phi());
			dijet_Minv = DiJet.M();
		}
		
		if(reweight) {
			weight = mcweight * expweight;
		} else {
			weight = mcweight;
		}
		
		weight /= luminosity;

		newtree->Fill();
	}

	newtree->AutoSave();
	delete in_file;
	delete out_file;
	delete lumi;
	return 0;
}
