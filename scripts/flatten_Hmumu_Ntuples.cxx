#include <TFile.h>
#include <TH1D.h>
#include <TTree.h>
#include <vector>

using namespace std;
int flatten(const char* in_file_name, const char* out_file_name)
{
	TFile* in_file = TFile::Open(in_file_name, "READ");
	const char* tree_name = "DiMuonNtuple";
	TTree* oldtree = (TTree*) in_file->Get(tree_name);
	// disbale all branches and only enable a bunch...
	// oldtree->SetBranchStatus("*", 0);
	// oldtree->SetBranchStatus("Muons_Charge_Lead", 1);
	// oldtree->SetBranchStatus("Muons_Minv_MuMu", 1);
	// oldtree->SetBranchStatus("Muons_PT_Lead", 1);
	// oldtree->SetBranchStatus("Muons_Eta_Lead", 1);
	// oldtree->SetBranchStatus("Muons_Phi_Lead", 1);
	// oldtree->SetBranchStatus("Muons_PT_Sub", 1);
	// oldtree->SetBranchStatus("Muons_Eta_Sub", 1);
	// oldtree->SetBranchStatus("Muons_Phi_Sub", 1);
	// oldtree->SetBranchStatus("Muons_Minv_MuMu_Fsr", 1);
	// oldtree->SetBranchStatus("Muons_Minv_MuMu_Sigma", 1);
	// oldtree->SetBranchStatus("Muons_Minv_MuMu_Fsr_Sigma", 1);
	// oldtree->SetBranchStatus("Muons_CosThetaStar", 1);
	// oldtree->SetBranchStatus("FSR_Et", 1);
	// oldtree->SetBranchStatus("FSR_Eta", 1);
	// oldtree->SetBranchStatus("FSR_Phi", 1);
	// oldtree->SetBranchStatus("Event_MET", 1);
	// oldtree->SetBranchStatus("Event_MET_Phi", 1);
	// oldtree->SetBranchStatus("Jets_PT", 1);
	// oldtree->SetBranchStatus("Jets_Eta", 1);
	// oldtree->SetBranchStatus("Jets_Phi", 1);
	// oldtree->SetBranchStatus("Jets_E", 1);

	Long64_t nentries = oldtree->GetEntries();
	TH1D* lumi = (TH1D*) in_file->Get("lumi");
	if(lumi) {
		lumi->SetDirectory(0);
	}

	vector<float>* jets_pt = new vector<float>();
	vector<float>* jets_eta = new vector<float>();
	vector<float>* jets_phi = new vector<float>();
	oldtree->SetBranchAddress("Jets_PT", &jets_pt);
	oldtree->SetBranchAddress("Jets_Eta", &jets_eta);
	oldtree->SetBranchAddress("Jets_Phi", &jets_phi);

	TFile* out_file = TFile::Open(out_file_name, "RECREATE");
	TTree* newtree = oldtree->CloneTree(0);
	float leading_jet_pT, leading_jet_eta, leading_jet_phi;
	float sub_jet_pT, sub_jet_eta, sub_jet_phi;
	if (lumi){
		lumi->Write();
	}

	newtree->Branch("Jets_PT_Lead", &leading_jet_pT, "Jets_PT_Lead/F");
	newtree->Branch("Jets_Eta_Lead", &leading_jet_eta, "Jets_Eta_Lead/F");
	newtree->Branch("Jets_Phi_Lead", &leading_jet_phi, "Jets_Phi_Lead/F");
	newtree->Branch("Jets_PT_Sub", &sub_jet_pT, "Jets_PT_Sub/F");
	newtree->Branch("Jets_Eta_Sub", &sub_jet_eta, "Jets_Eta_Sub/F");
	newtree->Branch("Jets_Phi_Sub", &sub_jet_phi, "Jets_Phi_Sub/F");
	
	for(Long64_t i=0; i < nentries; i++) {
		oldtree->GetEntry(i);
		if(jets_pt->size() > 0) {
			leading_jet_pT = jets_pt->at(0);
			leading_jet_eta = jets_eta->at(0);
			leading_jet_phi = jets_phi->at(0);
		} else {
			leading_jet_pT  = 0;
			leading_jet_eta = 0;
			leading_jet_phi = 0;
		}
		if(jets_pt->size() > 1) {
			sub_jet_pT = jets_pt->at(1);
			sub_jet_eta = jets_eta->at(1);
			sub_jet_phi = jets_phi->at(1);
		} else {
			sub_jet_pT  = 0;
			sub_jet_eta = 0;
			sub_jet_phi = 0;
		}
		newtree->Fill();
	}

	newtree->AutoSave();
	delete in_file;
	delete out_file;
	delete lumi;
	return 0;
}
