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
	Long64_t nentries = oldtree->GetEntries();
	TH1D* lumi = (TH1D*) in_file->Get("lumi");
	lumi->SetDirectory(0);

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
	lumi->Write();

	newtree->Branch("Leading_Jet_PT", &leading_jet_pT, "Leading_Jet_PT/F");
	newtree->Branch("Leading_Jet_Eta", &leading_jet_eta, "Leading_Jet_Eta/F");
	newtree->Branch("Leading_Jet_Phi", &leading_jet_phi, "Leading_Jet_Phi/F");
	newtree->Branch("Sub_Jet_PT", &sub_jet_pT, "Sub_Jet_PT/F");
	newtree->Branch("Sub_Jet_Eta", &sub_jet_eta, "Sub_Jet_Eta/F");
	newtree->Branch("Sub_Jet_Phi", &sub_jet_phi, "Sub_Jet_Phi/F");
	
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
