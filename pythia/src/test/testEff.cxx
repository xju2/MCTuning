#include <iostream>
#include <vector>
#include <math.h>

using namespace std;

void ELECTRON_IDEFF_ATLAS_RUN1_MEDIUM();
void ELECTRON_IDEFF_ATLAS_RUN2_LOOSE();
void ELECTRON_IDEFF_ATLAS_RUN1_TIGHT();
void MUON_SMEAR_ATLAS_RUN1();
void ELECTRON_SMEAR_ATLAS_RUN1_E2S();

int main(int argc, char** argv){
	// ELECTRON_IDEFF_ATLAS_RUN1_MEDIUM();
	// ELECTRON_IDEFF_ATLAS_RUN2_LOOSE(); 
	// ELECTRON_IDEFF_ATLAS_RUN1_TIGHT();
	// MUON_SMEAR_ATLAS_RUN1();
	ELECTRON_SMEAR_ATLAS_RUN1_E2S();
	return 0;
}

void ELECTRON_IDEFF_ATLAS_RUN1_MEDIUM() {

	vector<double> eta_edges_10 = {0.000, 0.049, 0.454, 1.107, 1.46, 1.790, 2.277, 2.500};
	vector<double> eta_vals_10  = {0.730, 0.757, 0.780, 0.771, 0.77, 0.777, 0.778};
	
	vector<double> eta_edges_15 = {0.000, 0.053, 0.456, 1.102, 1.463, 1.783, 2.263, 2.500};
	vector<double> eta_vals_15  = {0.780, 0.800, 0.819, 0.759, 0.749, 0.813, 0.829};
	
	vector<double> eta_edges_20 = {0.000, 0.065, 0.362, 0.719, 0.980, 1.289, 1.455, 1.681, 1.942, 2.239, 2.452, 2.500};
	vector<double> eta_vals_20  = {0.794, 0.806, 0.816, 0.806, 0.797, 0.774, 0.764, 0.788, 0.793, 0.806, 0.825};
	
	vector<double> eta_edges_25 = {0.000, 0.077, 0.338, 0.742, 1.004, 1.265, 1.467, 1.692, 1.940, 2.227, 2.452, 2.500};
	vector<double> eta_vals_25  = {0.833, 0.843, 0.853, 0.845, 0.839, 0.804, 0.790, 0.825, 0.830, 0.833, 0.839};
	
	vector<double> eta_edges_30 = {0.000, 0.077, 0.350, 0.707, 0.980, 1.289, 1.479, 1.681, 1.942, 2.239, 2.441, 2.500};
	vector<double> eta_vals_30  = {0.863, 0.872, 0.881, 0.874, 0.870, 0.824, 0.808, 0.847, 0.845, 0.840, 0.842};
	
	vector<double> eta_edges_35 = {0.000, 0.058, 0.344, 0.700, 1.009, 1.270, 1.458, 1.685, 1.935, 2.231, 2.468, 2.500};
	vector<double> eta_vals_35  = {0.878, 0.889, 0.901, 0.895, 0.893, 0.849, 0.835, 0.868, 0.863, 0.845, 0.832};
	
	vector<double> eta_edges_40 = {0.000, 0.047, 0.355, 0.699, 0.983, 1.280, 1.446, 1.694, 1.943, 2.227, 2.441, 2.500};
	vector<double> eta_vals_40  = {0.894, 0.901, 0.909, 0.905, 0.904, 0.875, 0.868, 0.889, 0.876, 0.848, 0.827};
	
	vector<double> eta_edges_45 = {0.000, 0.058, 0.356, 0.712, 0.997, 1.282, 1.459, 1.686, 1.935, 2.220, 2.444, 2.500};
	vector<double> eta_vals_45  = {0.900, 0.911, 0.923, 0.918, 0.917, 0.897, 0.891, 0.904, 0.894, 0.843, 0.796};
	
	vector<double> eta_edges_50 = {0.000, 0.059, 0.355, 0.711, 0.983, 1.280, 1.469, 1.682, 1.919, 2.227, 2.441, 2.500};
	vector<double> eta_vals_50  = {0.903, 0.913, 0.923, 0.922, 0.923, 0.903, 0.898, 0.908, 0.895, 0.831, 0.774};
	
	vector<double> eta_edges_60 = {0.000, 0.053, 0.351, 0.720, 1.006, 1.291, 1.469, 1.696, 1.946, 2.243, 2.455, 2.500};
	vector<double> eta_vals_60  = {0.903, 0.917, 0.928, 0.924, 0.927, 0.915, 0.911, 0.915, 0.899, 0.827, 0.760};
	
	vector<double> eta_edges_80 = {0.000, 0.053, 0.351, 0.720, 0.994, 1.292, 1.482, 1.708, 1.934, 2.220, 2.458, 2.500};
	vector<double> eta_vals_80  = {0.936, 0.942, 0.952, 0.956, 0.956, 0.934, 0.931, 0.944, 0.933, 0.940, 0.948};
	
	vector<double> et_edges = { 10, 15, 20, 25, 30, 35, 40, 45, 50, 60, 80, 1000 };
	vector< vector<double> > et_eta_edges = { eta_edges_10, eta_edges_15, eta_edges_20, eta_edges_25, eta_edges_30, eta_edges_35, eta_edges_40, eta_edges_45, eta_edges_50, eta_edges_60, eta_edges_80 };
	vector< vector<double> > et_eta_vals  = { eta_vals_10, eta_vals_15, eta_vals_20, eta_vals_25, eta_vals_30, eta_vals_35, eta_vals_40, eta_vals_45, eta_vals_50, eta_vals_60, eta_vals_80 };
	
	for(int i = 0; i < (int)et_edges.size()-1; i++){
		vector<double> values = et_eta_vals[i];
		vector<double> eta_bins = et_eta_edges[i];
		for(int j = 0; j < (int) eta_bins.size() - 1; j++){
			cout << eta_bins.at(j) <<"\t" << eta_bins.at(j+1) 
				<< "\t" << et_edges.at(i) << "\t" << et_edges.at(i+1) << "\t"
				<< values.at(j) << "\t 0\t1\t0\t1" << endl;
		}
	}
}

void ELECTRON_IDEFF_ATLAS_RUN2_LOOSE() 
{

    // Manually symmetrised eta eff histogram
	vector<double> edges_eta = { 0.0,   0.1,   0.8,   1.37,  1.52,  2.01,  2.37,  2.47 };
	vector<double> effs_eta  = { 0.950, 0.965, 0.955, 0.885, 0.950, 0.935, 0.90 };
	vector<double> edges_et = {  10,   20,   25,   30,   35,   40,    45,    50,   60,  80, 10000};
	vector<double> effs_et  = { 0.90, 0.91, 0.92, 0.94, 0.95, 0.955, 0.965, 0.97, 0.98, 0.98 };

	for(int i=0; i < (int)edges_et.size()-1; i++){
		for(int j=0; j < (int) edges_eta.size()-1; j++){
			double et = edges_et.at(j);
			double eff = effs_eta[j] * effs_et[i] / 0.95;
			cout << edges_eta.at(j) << "\t" << edges_eta.at(j+1) << "\t"
				<< edges_et.at(i) << "\t" << edges_et.at(i+1) << "\t"
				<< min(eff, 1.0) << "\t 0\t1\t0\t1" << endl; 
		}
	}
}

  /// @brief ATLAS Run 1 'tight' electron identification/selection efficiency
void ELECTRON_IDEFF_ATLAS_RUN1_TIGHT() 
{

	vector<double> eta_edges_10 = {0.000, 0.049, 0.459, 1.100, 1.461, 1.789, 2.270, 2.500};
	vector<double> eta_vals_10  = {0.581, 0.632, 0.668, 0.558, 0.548, 0.662, 0.690};
	
	vector<double> eta_edges_15 = {0.000, 0.053, 0.450, 1.096, 1.463, 1.783, 2.269, 2.500};
	vector<double> eta_vals_15 =  {0.630, 0.678, 0.714, 0.633, 0.616, 0.700, 0.733};
	
	vector<double> eta_edges_20 = {0.000, 0.065, 0.362, 0.719, 0.992, 1.277, 1.479, 1.692, 1.930, 2.227, 2.464, 2.500};
	vector<double> eta_vals_20 =  {0.653, 0.695, 0.735, 0.714, 0.688, 0.635, 0.625, 0.655, 0.680, 0.691, 0.674};
	
	vector<double> eta_edges_25 = {0.000, 0.077, 0.362, 0.719, 0.992, 1.300, 1.479, 1.692, 1.942, 2.227, 2.464, 2.500};
	vector<double> eta_vals_25 =  {0.692, 0.732, 0.768, 0.750, 0.726, 0.677, 0.667, 0.692, 0.710, 0.706, 0.679};
	
	vector<double> eta_edges_30 = {0.000, 0.053, 0.362, 0.719, 1.004, 1.277, 1.467, 1.681, 1.954, 2.239, 2.452, 2.500};
	vector<double> eta_vals_30 =  {0.724, 0.763, 0.804, 0.789, 0.762, 0.702, 0.690, 0.720, 0.731, 0.714, 0.681};
	
	vector<double> eta_edges_35 = {0.000, 0.044, 0.342, 0.711, 0.971, 1.280, 1.456, 1.683, 1.944, 2.218, 2.442, 2.500};
	vector<double> eta_vals_35 =  {0.736, 0.778, 0.824, 0.811, 0.784, 0.730, 0.718, 0.739, 0.743, 0.718, 0.678};
	
	vector<double> eta_edges_40 = {0.000, 0.047, 0.355, 0.699, 0.983, 1.268, 1.457, 1.671, 1.931, 2.204, 2.453, 2.500};
	vector<double> eta_vals_40 =  {0.741, 0.774, 0.823, 0.823, 0.802, 0.764, 0.756, 0.771, 0.771, 0.734, 0.684};
	
	vector<double> eta_edges_45 = {0.000, 0.056, 0.354, 0.711, 0.984, 1.280, 1.458, 1.684, 1.945, 2.207, 2.442, 2.500};
	vector<double> eta_vals_45 =  {0.758, 0.792, 0.841, 0.841, 0.823, 0.792, 0.786, 0.796, 0.794, 0.734, 0.663};
	
	vector<double> eta_edges_50 = {0.000, 0.059, 0.355, 0.699, 0.983, 1.268, 1.446, 1.682, 1.943, 2.216, 2.453, 2.500};
	vector<double> eta_vals_50 =  {0.771, 0.806, 0.855, 0.858, 0.843, 0.810, 0.800, 0.808, 0.802, 0.730, 0.653};
	
	vector<double> eta_edges_60 = {0.000, 0.050, 0.350, 0.707, 0.981, 1.278, 1.468, 1.694, 1.944, 2.242, 2.453, 2.500};
	vector<double> eta_vals_60 =  {0.773, 0.816, 0.866, 0.865, 0.853, 0.820, 0.812, 0.817, 0.804, 0.726, 0.645};
	
	vector<double> eta_edges_80 = {0.000, 0.051, 0.374, 0.720, 0.981, 1.279, 1.468, 1.707, 1.945, 2.207, 2.457, 2.500};
	vector<double> eta_vals_80 =  {0.819, 0.855, 0.899, 0.906, 0.900, 0.869, 0.865, 0.873, 0.869, 0.868, 0.859};
	
	vector<double> et_edges = { 10, 15, 20, 25, 30, 35, 40, 45, 50, 60, 80, 1000 };
	vector< vector<double> > et_eta_edges = { eta_edges_10, eta_edges_15, eta_edges_20, eta_edges_25, eta_edges_30, eta_edges_35, eta_edges_40, eta_edges_45, eta_edges_50, eta_edges_60, eta_edges_80 };
    const static vector< vector<double> > et_eta_vals  = { eta_vals_10, eta_vals_15, eta_vals_20, eta_vals_25, eta_vals_30, eta_vals_35, eta_vals_40, eta_vals_45, eta_vals_50, eta_vals_60, eta_vals_80 };

	for(int i = 0; i < (int)et_edges.size()-1; i++){
		vector<double> values = et_eta_vals[i];
		vector<double> eta_bins = et_eta_edges[i];
		for(int j = 0; j < (int) eta_bins.size() - 1; j++){
			cout << eta_bins.at(j) <<"\t" << eta_bins.at(j+1) 
				<< "\t" << et_edges.at(i) << "\t" << et_edges.at(i+1) << "\t"
				<< values.at(j) << "\t 0\t1\t0\t1" << endl;
		}
	}

}

void MUON_SMEAR_ATLAS_RUN1() 
{
	vector<double> edges_eta = {0, 1.5, 2.5};
	vector<double> edges_pt = {0.1, 1.0, 10., 200., 5000};
	vector<double> res = {0.03, 0.02, 0.03, 0.05,
						  0.04, 0.03, 0.04, 0.05};

	for(int i = 0; i < (int) edges_pt.size() -1; i ++){
		for(int j = 0; j < (int) edges_eta.size()-1; j++){
			int ibin = j*(edges_pt.size() -1) + i;
			cout << edges_eta.at(j) <<"\t" << edges_eta.at(j+1) 
				<< "\t" << edges_pt.at(i) << "\t" << edges_pt.at(i+1) << "\t"
				<< res.at(ibin) << "\t 0\t1\t0\t1" << endl;
		}
  }
}


void ELECTRON_SMEAR_ATLAS_RUN1_E2S()
{
    const vector<double> edges_eta = {0., 2.5, 3., 5.};
    const vector<double> edges_pt = {0., 0.1, 25., 10000};
    const vector<double> e2s = {0.000, 0.015, 0.005,
                                0.005, 0.005, 0.005,
                                0.107, 0.107, 0.107};
    const vector<double> es = {0.00, 0.00, 0.05,
                               0.05, 0.05, 0.05,
                               2.08, 2.08, 2.08};
    const vector<double> cs = {0.00, 0.00, 0.25,
                                      0.25, 0.25, 0.25,
                                      0.00, 0.00, 0.00};

	for(int i = 0; i < (int) edges_pt.size() -1; i ++){
		for(int j = 0; j < (int) edges_eta.size()-1; j++){
			int ibin = j*(edges_pt.size() -1) + i;
			
			cout << edges_eta.at(j) <<"\t " << edges_eta.at(j+1) 
				<< "\t " << edges_pt.at(i) << "\t " << edges_pt.at(i+1) << "\t "
				<< cs.at(ibin) << "\t 0\t 1\t 0\t 1" << endl;
		}
  }

}
