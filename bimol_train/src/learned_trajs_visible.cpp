#include <dynamicboltz>

#include <iostream>
#include <fstream>
#include <sstream>

using namespace dblz;
using namespace std;
using namespace q3c1;

// Read init conds
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

int main() {

	/****************************************
	Make some species
	****************************************/

	cout << "--- Making species ---" << endl;

	auto species_A = make_shared<Species>("A");

	cout << "--- [Finished] Making species ---" << endl;
	cout << endl;

	/****************************************
	Make ixn
	****************************************/

	cout << "--- Making ixn func ---" << endl;

	auto ixn_hA = make_shared<IxnParam>("hA",IxnParamType::H, 0.0);
	auto ixn_jAA = make_shared<IxnParam>("jAA",IxnParamType::J, 0.0);
	auto ixn_kAAA = make_shared<IxnParam>("kAAA",IxnParamType::K, 0.0);

	cout << "--- [Finished] Making ixn func ---" << endl;
	cout << endl;

	/****************************************
	Make RHS diff eq for ixn
	****************************************/

	cout << "--- Making RHS diff eq ---" << endl;

	// Domain
	Domain1D domain_1d_hA = Domain1D(ixn_hA,-2.0,1.2,6);
	Domain1D domain_1d_jAA = Domain1D(ixn_jAA,-1.2,1.2,6);
	Domain1D domain_1d_kAAA = Domain1D(ixn_kAAA,-1.2,1.2,6);
	std::vector<Domain1D*> domain({&domain_1d_hA,&domain_1d_jAA,&domain_1d_kAAA});

	// RHS
	auto rhs_hA = make_shared<DiffEqRHS>("DE hA", ixn_hA, domain);
	ixn_hA->set_diff_eq_rhs(rhs_hA);
	auto rhs_jAA = make_shared<DiffEqRHS>("DE jAA", ixn_jAA, domain);
	ixn_jAA->set_diff_eq_rhs(rhs_jAA);
	auto rhs_kAAA = make_shared<DiffEqRHS>("DE kAAA", ixn_kAAA, domain);
	ixn_kAAA->set_diff_eq_rhs(rhs_kAAA);

	cout << "--- [Finished] Making RHS diff eq ---" << endl;
	cout << endl;

	/********************
	Read in
	********************/

	rhs_hA->read_from_file("data_learned_visible/diff_eq_rhs/hA_0190.txt");
	rhs_jAA->read_from_file("data_learned_visible/diff_eq_rhs/jAA_0190.txt");
	rhs_kAAA->read_from_file("data_learned_visible/diff_eq_rhs/kAAA_0190.txt");

	/****************************************
	Go through all IC
	****************************************/

	int no_timesteps = 100;
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

		auto init_cond_vector = read_init_conds("../bimol_train_stoch_sim/ic_v"+pad_str(IC,3)+"/init_visible.txt");

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

		ixn_hA->write_to_file("learned_trajs_visible/"+ixn_hA->get_name()+"_"+pad_str(IC,3)+".txt");
		ixn_jAA->write_to_file("learned_trajs_visible/"+ixn_jAA->get_name()+"_"+pad_str(IC,3)+".txt");
		ixn_kAAA->write_to_file("learned_trajs_visible/"+ixn_kAAA->get_name()+"_"+pad_str(IC,3)+".txt");
	};

	return 0;
};