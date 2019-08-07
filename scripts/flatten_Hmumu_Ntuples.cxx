#include <TFile.h>
#include <TH1D.h>
#include <TTree.h>
#include <vector>

using namespace std;
int flatten(const char* in_file_name, const char* out_file_name, bool reweight=true)
{
	TFile* in_file = TFile::Open(in_file_name, "READ");
	const char* tree_name = "DiMuonNtuple";
	TTree* oldtree = (TTree*) in_file->Get(tree_name);

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
	oldtree->SetBranchAddress("Jets_PT", &jets_pt);
	oldtree->SetBranchAddress("Jets_Eta", &jets_eta);
	oldtree->SetBranchAddress("Jets_Phi", &jets_phi);
	oldtree->SetBranchAddress("McEventWeight", &mcweight);
	oldtree->SetBranchAddress("ExpWeight", &expweight);

	TFile* out_file = TFile::Open(out_file_name, "RECREATE");
	TTree* newtree = oldtree->CloneTree(0);
	if (lumi){
		lumi->Write();
	}
	double luminosity = lumi->GetBinContent(1);
	
	// additional variables
	float weight;
	float leading_jet_pT, leading_jet_eta, leading_jet_phi;
	float sub_jet_pT, sub_jet_eta, sub_jet_phi;
	int njets = 0;
	newtree->Branch("Jets_PT_Lead", &leading_jet_pT, "Jets_PT_Lead/F");
	newtree->Branch("Jets_Eta_Lead", &leading_jet_eta, "Jets_Eta_Lead/F");
	newtree->Branch("Jets_Phi_Lead", &leading_jet_phi, "Jets_Phi_Lead/F");
	newtree->Branch("Jets_PT_Sub", &sub_jet_pT, "Jets_PT_Sub/F");
	newtree->Branch("Jets_Eta_Sub", &sub_jet_eta, "Jets_Eta_Sub/F");
	newtree->Branch("Jets_Phi_Sub", &sub_jet_phi, "Jets_Phi_Sub/F");
	newtree->Branch("weight", &weight, "weight/F");
	newtree->Branch("Jets_jetMultip", &njets, "Jets_jetMultip/I");
	
	for(Long64_t i=0; i < nentries; i++) {
		oldtree->GetEntry(i);
		njets = jets_pt->size();
		if(jets_pt->size() > 0) {
			leading_jet_pT = jets_pt->at(0);
			leading_jet_eta = jets_eta->at(0);
			leading_jet_phi = jets_phi->at(0);
		} else {
			leading_jet_pT  = -9999;
			leading_jet_eta = -9999;
			leading_jet_phi = -9999;
		}
		if(jets_pt->size() > 1) {
			sub_jet_pT = jets_pt->at(1);
			sub_jet_eta = jets_eta->at(1);
			sub_jet_phi = jets_phi->at(1);
		} else {
			sub_jet_pT  = -9999;
			sub_jet_eta = -9999;
			sub_jet_phi = -9999;
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
