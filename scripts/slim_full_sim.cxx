#include <TFile.h>
#include <TH1D.h>
#include <TTree.h>
#include <vector>

using namespace std;
int slim(const char* in_file_name, const char* out_file_name)
{
	TFile* in_file = TFile::Open(in_file_name, "READ");
	const char* tree_name = "DiMuonNtuple";
	TTree* oldtree = (TTree*) in_file->Get(tree_name);
	// disbale all branches and only enable a bunch...
	oldtree->SetBranchStatus("*", 0);
	oldtree->SetBranchStatus("Muons_Charge_Lead", 1);
	oldtree->SetBranchStatus("Muons_Minv_MuMu", 1);
	oldtree->SetBranchStatus("Muons_PT_Lead", 1);
	oldtree->SetBranchStatus("Muons_Eta_Lead", 1);
	oldtree->SetBranchStatus("Muons_Phi_Lead", 1);
	oldtree->SetBranchStatus("Muons_PT_Sub", 1);
	oldtree->SetBranchStatus("Muons_Eta_Sub", 1);
	oldtree->SetBranchStatus("Muons_Phi_Sub", 1);
	oldtree->SetBranchStatus("Jets_PT_Lead", 1);
	oldtree->SetBranchStatus("Jets_Eta_Lead", 1);
	oldtree->SetBranchStatus("Jets_Phi_Lead", 1);
	oldtree->SetBranchStatus("Jets_PT_Sub", 1);
	oldtree->SetBranchStatus("Jets_Eta_Sub", 1);
	oldtree->SetBranchStatus("Jets_Phi_Sub", 1);
	oldtree->SetBranchStatus("Muons_Minv_MuMu", 1);
	oldtree->SetBranchStatus("Muons_Minv_MuMu_Fsr", 1);
	oldtree->SetBranchStatus("Muons_Minv_MuMu_Sigma", 1);
	oldtree->SetBranchStatus("Muons_CosThetaStar", 1);
	oldtree->SetBranchStatus("FSR_Et", 1);
	oldtree->SetBranchStatus("FSR_Eta", 1);
	oldtree->SetBranchStatus("FSR_Phi", 1);
	oldtree->SetBranchStatus("Event_MET", 1);
	oldtree->SetBranchStatus("Event_MET_Phi", 1);
	oldtree->SetBranchStatus("Truth_Eta_Lead_Muon", 1);
	oldtree->SetBranchStatus("Truth_Phi_Lead_Muon", 1);
	oldtree->SetBranchStatus("Truth_PT_Lead_Muon", 1);
	oldtree->SetBranchStatus("Truth_Eta_Sub_Muon", 1);
	oldtree->SetBranchStatus("Truth_Phi_Sub_Muon", 1);
	oldtree->SetBranchStatus("Truth_PT_Sub_Muon", 1);
	oldtree->SetBranchStatus("Truth_Minv_MuMu", 1);

	oldtree->SetBranchStatus("GlobalWeight", 1);
	oldtree->SetBranchStatus("SampleOverlapWeight", 1);
	oldtree->SetBranchStatus("EventWeight_MCCleaning_5", 1);

	oldtree->SetBranchStatus("FinalSelection", 1);

	// For Event Selections
	//oldtree->SetBranchStatus("EventInfo_PassTrigger", 1);
	//oldtree->SetBranchStatus("EventInfo_PassTriggerMatching", 1);
	//oldtree->SetBranchStatus("Event_HasBJet", 1);
	//oldtree->SetBranchStatus("Muons_Multip", 1);

	Long64_t nentries = oldtree->GetEntries();

	float global_weight;	
	float sample_overlap;	
	float mcclean;	
	int pass_trigger;
	int pass_trigger_matching;
	int has_bjet;
	int muons_multip;
	float muon_pt_lead;
	oldtree->SetBranchAddress("GlobalWeight", &global_weight);
	oldtree->SetBranchAddress("SampleOverlapWeight", &sample_overlap);
	oldtree->SetBranchAddress("EventWeight_MCCleaning_5", &mcclean);

	float pass_sel;
	oldtree->SetBranchAddress("FinalSelection", &pass_sel);
	//oldtree->SetBranchAddress("Muons_PT_Lead", &muon_pt_lead);
	//oldtree->SetBranchAddress("EventInfo_PassTrigger", &pass_trigger);
	//oldtree->SetBranchAddress("EventInfo_PassTriggerMatching", &pass_trigger_matching);
	//oldtree->SetBranchAddress("Event_HasBJet", &has_bjet);
	//oldtree->SetBranchAddress("Muons_Multip", &muons_multip);

	TFile* out_file = TFile::Open(out_file_name, "RECREATE");
	TTree* newtree = oldtree->CloneTree(0);

	float mcweights;
	newtree->Branch("McEventWeight", &mcweights, "McEventWeight/F");
	
	for(Long64_t i=0; i < nentries; i++) {
		oldtree->GetEntry(i);
		mcweights = global_weight * sample_overlap * mcclean;
		if(pass_sel!=1) continue;
		//if(muon_pt_lead <= 27) continue;
		//if(pass_trigger != 1 || pass_trigger_matching != 1 || has_bjet != 0) continue;
		//if(muons_multip > 2) continue;

		newtree->Fill();
	}

	newtree->AutoSave();
	delete in_file;
	delete out_file;
	return 0;
}
