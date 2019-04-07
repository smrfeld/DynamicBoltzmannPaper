#include <dblz>

#include <iostream>
#include <fstream>
#include <sstream>

using namespace dblz;
using namespace std;
using namespace q3c1;

// ***************
// MARK: - Read init conds
// ***************

vector<double> read_init_conds(string fname) {
	vector<double> ret;

	std::ifstream f;
	f.open(fname);
	std::string nu="";
	std::string val="";
	std::string line;
	std::istringstream iss;
	if (f.is_open()) { // make sure we found it
		while (getline(f,line)) {
			if (line == "") { continue; };
			iss = std::istringstream(line);
		    iss >> nu;
		    iss >> val;
		    ret.push_back(atof(val.c_str()));
		    nu=""; val="";
		};
	};
	f.close();

	return ret;
};

// ***************
// MARK: - Main
// ***************

int main() {

    // ***************
    // MARK: - Make some species
    // ***************
    
    cout << "--- Making species ---" << endl;
    
    auto species_A = make_shared<Species>("A");
    vector<shared_ptr<Species>> sp_vis({species_A});
    
    cout << "--- [Finished] Making species ---" << endl;
    cout << endl;
    
    // ***************
    // MARK: - Make ixns
    // ***************
    
    cout << "--- Making ixn func ---" << endl;
    
    // Init cond
    double init_cond = 0.0;
    
    auto ixn_hA = make_shared<IxnParamTraj>("hA",IxnParamType::H, init_cond);
    auto ixn_jAA = make_shared<IxnParamTraj>("jAA",IxnParamType::J, init_cond);
    auto ixn_kAAA = make_shared<IxnParamTraj>("kAAA",IxnParamType::K, init_cond);
    
    cout << "--- [Finished] Making ixn func ---" << endl;
    cout << endl;
    
    // ***************
    // MARK: - Make RHS diff eqs
    // ***************
    
    cout << "--- Making RHS diff eq ---" << endl;
    
    // Domain
    double spacing = 0.5; // 0.4
    double center = 0.0;
    Domain1D* domain_1d_hA = new Domain1D(ixn_hA,spacing,center);
    Domain1D* domain_1d_jAA = new Domain1D(ixn_jAA,spacing,center);
    Domain1D* domain_1d_kAAA = new Domain1D(ixn_kAAA,spacing,center);
    auto domain = std::vector<Domain1D*>({domain_1d_hA,domain_1d_jAA,domain_1d_kAAA});
    
    // Learning rate
    double dopt = 1.0;
    
    // RHS
    auto rhs_hA = make_shared<DiffEqRHS>("DE hA", ixn_hA, domain, dopt);
    ixn_hA->set_diff_eq_rhs(rhs_hA);
    auto rhs_jAA = make_shared<DiffEqRHS>("DE jAA", ixn_jAA, domain, dopt);
    ixn_jAA->set_diff_eq_rhs(rhs_jAA);
    auto rhs_kAAA = make_shared<DiffEqRHS>("DE kAAA", ixn_kAAA, domain, dopt);
    ixn_kAAA->set_diff_eq_rhs(rhs_kAAA);
    
    // Dependency
    ixn_hA->add_diff_eq_dependency(ixn_hA->get_diff_eq_rhs(), 0);
    ixn_hA->add_diff_eq_dependency(ixn_jAA->get_diff_eq_rhs(), 0);
    ixn_hA->add_diff_eq_dependency(ixn_kAAA->get_diff_eq_rhs(), 0);
    ixn_jAA->add_diff_eq_dependency(ixn_hA->get_diff_eq_rhs(), 1);
    ixn_jAA->add_diff_eq_dependency(ixn_jAA->get_diff_eq_rhs(), 1);
    ixn_jAA->add_diff_eq_dependency(ixn_kAAA->get_diff_eq_rhs(), 1);
    ixn_kAAA->add_diff_eq_dependency(ixn_hA->get_diff_eq_rhs(), 2);
    ixn_kAAA->add_diff_eq_dependency(ixn_jAA->get_diff_eq_rhs(), 2);
    ixn_kAAA->add_diff_eq_dependency(ixn_kAAA->get_diff_eq_rhs(), 2);
    
    cout << "--- [Finished] Making RHS diff eq ---" << endl;
    cout << endl;
    
    // ***************
    // MARK: - Setup lattice
    // ***************
    
    cout << "--- Making lattice ---" << endl;
    
    // 1000 cube
    bool conn_nn = true;
    bool conn_triplet = true;
    auto latt = make_shared<LatticeTraj1DFullyVisible>(1000,sp_vis,conn_nn,conn_triplet);
    
    cout << " > begin visible" << endl;
    
    // Set bias
    latt->set_bias(species_A, ixn_hA);
    
    // NNs
    latt->set_ixn_2(species_A, species_A, ixn_jAA);
    
    // Triplets
    latt->set_ixn_3(species_A, species_A, species_A, ixn_kAAA);
    
    cout << " > end visible" << endl;

    // ***************
    // MARK: - Read in
    // ***************
    
    int idx_read = 190;
	rhs_hA->read_from_file("../data_learned_visible/diff_eq_rhs/hA_"+pad_str(idx_read,4)+".txt");
	rhs_jAA->read_from_file("../data_learned_visible/diff_eq_rhs/jAA_"+pad_str(idx_read,4)+".txt");
	rhs_kAAA->read_from_file("../data_learned_visible/diff_eq_rhs/kAAA_"+pad_str(idx_read,4)+".txt");

	/****************************************
	Go through all IC
	****************************************/

	int no_timesteps = 200;
	double dt=0.01;

	// Init structure
	ixn_hA->set_no_timesteps(no_timesteps);
	ixn_jAA->set_no_timesteps(no_timesteps);
	ixn_kAAA->set_no_timesteps(no_timesteps);

	for (int IC=1; IC<=50; IC++) {

		std::cout << "IC: " << IC << std::endl;

		/********************
		Read init cond
		********************/

		auto init_cond_vector = read_init_conds("../../bimol_train_stoch_sim/data/ic_v"+pad_str(IC,3)+"/init_visible.txt");

		// Set
		ixn_hA->set_init_cond(init_cond_vector[0]);
		ixn_jAA->set_init_cond(init_cond_vector[1]);
		ixn_kAAA->set_init_cond(init_cond_vector[2]);

		/********************
		Solve
		********************/
		
		for (auto t=0; t<no_timesteps; t++) {
			ixn_hA->solve_diff_eq_at_timepoint_to_plus_one(t,dt);
			ixn_jAA->solve_diff_eq_at_timepoint_to_plus_one(t,dt);
			ixn_kAAA->solve_diff_eq_at_timepoint_to_plus_one(t,dt);
		};

		/********************
		Write
		********************/

        ixn_hA->write_val_traj_to_file(0, no_timesteps, "../data_learned_visible/trajs/"+ixn_hA->get_name()+"_"+pad_str(IC,3)+".txt");
        ixn_jAA->write_val_traj_to_file(0, no_timesteps, "../data_learned_visible/trajs/"+ixn_jAA->get_name()+"_"+pad_str(IC,3)+".txt");
        ixn_kAAA->write_val_traj_to_file(0, no_timesteps, "../data_learned_visible/trajs/"+ixn_kAAA->get_name()+"_"+pad_str(IC,3)+".txt");
	};

	return 0;
};
