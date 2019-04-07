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
    // MARK: - Adjoint
    // ***************
    
	cout << "--- Making adjoint ---" << endl;

	auto adjoint_hA = make_shared<Adjoint>("adjoint hA",ixn_hA);
	ixn_hA->set_adjoint(adjoint_hA);

	auto adjoint_jAA = make_shared<Adjoint>("adjoint jAA",ixn_jAA);
	ixn_jAA->set_adjoint(adjoint_jAA);

	auto adjoint_kAAA = make_shared<Adjoint>("adjoint kAAA",ixn_kAAA);
	ixn_kAAA->set_adjoint(adjoint_kAAA);

	cout << "--- [Finished] Making adjoint ---" << endl;
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

	/****************************************
	Setup optimizer
	****************************************/

	// Opt solver class
	OptProblemDynamic opt(latt);

	// Params
	int no_opt_steps = 190;
    
	double dt=0.01;
	int no_cd_steps = 1;
    
    // Batch size
	int batch_size = 10;
    latt->set_no_markov_chains(MCType::AWAKE, batch_size);
    latt->set_no_markov_chains(MCType::ASLEEP, batch_size);

	// Options
	OptionsSolveDynamic options;
    options.solver = Solver::SGD;

    // Options wake sleep
    OptionsWakeSleep_1DFV_CD options_wake_sleep;
	options_wake_sleep.verbose_timing = false;

    // Dir to write to
    std::string dir_write = "../data_learned_visible";

    // No timesteps for ixn params
    int no_timesteps = 200;
    ixn_hA->set_no_timesteps(no_timesteps);
    ixn_jAA->set_no_timesteps(no_timesteps);
    ixn_kAAA->set_no_timesteps(no_timesteps);

    // No timesteps
    int timepoint_start_latt = 0;
    int no_timesteps_latt = 5;
    latt->set_no_timesteps(timepoint_start_latt, no_timesteps_latt);
    
	/********************
	Read init cond
	********************/

	std::map<int,double> ixn_hA_IC, ixn_jAA_IC, ixn_kAAA_IC;
	for (auto IC=1; IC<=50; IC++) {
		auto init_cond_vector = read_init_conds("../../bimol_train_stoch_sim/data/ic_v"+pad_str(IC,3)+"/init_visible.txt");

		ixn_hA_IC[IC] = init_cond_vector[0];
		ixn_jAA_IC[IC] = init_cond_vector[1];
		ixn_kAAA_IC[IC] = init_cond_vector[2];
	};

	/********************
	Filenames
	********************/

	int no_timesteps_max = 200;

    std::map<int, FNameTrajColl> fnames_map;
    for (auto IC=1; IC<=50; IC++) {
        fnames_map[IC] = FNameTrajColl();
        
        for (auto i_batch=1; i_batch<=50; i_batch++) {
            
            // Series of filenames
            FNameTraj fname_traj;
            for (auto timepoint=0; timepoint<=no_timesteps_max; timepoint++) {
                fname_traj.push_back(FName("../../bimol_train_stoch_sim/data/ic_v"+pad_str(IC,3)+"/lattice_v"+pad_str(i_batch,3)+"/lattice/"+pad_str(timepoint,4)+".txt",true));
            };
            
            // Add
            fnames_map[IC].add_fname_traj(fname_traj);
        };
    };

	/********************
	Iterate over opt steps
	********************/

    int timepoint_start_solve_ixn_params = 0;
    int no_timesteps_solve_ixns_params = 5;

	for (auto opt_step=1; opt_step<=no_opt_steps; opt_step++) {

		// Gradually increase window
        if (opt_step % 2 == 1 && opt_step != 1) {
            timepoint_start_latt += 1;
            no_timesteps_solve_ixns_params += 1;
            latt->set_no_timesteps(timepoint_start_latt, no_timesteps_latt);
		};

		// All ICS
		for (auto IC=1; IC<=50; IC++) {

			std::cout << "------------------------------------" << std::endl;
			std::cout << "Opt step: " << opt_step << " / " << no_opt_steps << "; init cond: " << IC << std::endl;
			std::cout << "------------------------------------" << std::endl;

			// Set init cond
			ixn_hA->set_init_cond(ixn_hA_IC[IC]);
			ixn_jAA->set_init_cond(ixn_jAA_IC[IC]);
			ixn_kAAA->set_init_cond(ixn_kAAA_IC[IC]);

			/********************
			Solve one step
			********************/

			// Take a step
            opt.solve_one_step_1d_fully_visible(opt_step, timepoint_start_solve_ixn_params, no_timesteps_solve_ixns_params, timepoint_start_latt, no_timesteps_latt, timepoint_start_latt, no_timesteps_latt, dt, no_cd_steps, fnames_map.at(IC), options, options_wake_sleep);

            // Write moments
            /*
            ixn_hA->write_moment_traj_to_file(timepoint_start_latt, no_timesteps_latt, dir_write+"/moments/"+ixn_hA->get_name()+"_"+pad_str(IC,3)+"_"+pad_str(opt_step,4)+".txt");
            ixn_jAA->write_moment_traj_to_file(timepoint_start_latt, no_timesteps_latt, dir_write+"/moments/"+ixn_jAA->get_name()+"_"+pad_str(IC,3)+"_"+pad_str(opt_step,4)+".txt");
            ixn_kAAA->write_moment_traj_to_file(timepoint_start_latt, no_timesteps_latt, dir_write+"/moments/"+ixn_kAAA->get_name()+"_"+pad_str(IC,3)+"_"+pad_str(opt_step,4)+".txt");
            
            // Write ixn params
            ixn_hA->write_val_traj_to_file(timepoint_start_latt, no_timesteps_latt, dir_write+"/ixn_params/"+ixn_hA->get_name()+"_"+pad_str(IC,3)+"_"+pad_str(opt_step,4)+".txt");
            ixn_jAA->write_val_traj_to_file(timepoint_start_latt, no_timesteps_latt, dir_write+"/ixn_params/"+ixn_jAA->get_name()+"_"+pad_str(IC,3)+"_"+pad_str(opt_step,4)+".txt");
            ixn_kAAA->write_val_traj_to_file(timepoint_start_latt, no_timesteps_latt, dir_write+"/ixn_params/"+ixn_kAAA->get_name()+"_"+pad_str(IC,3)+"_"+pad_str(opt_step,4)+".txt");
            */
			// Write adjoint
			/*
			ixn_hA->get_adjoint()->write_to_file("data_learned_visible/adjoint/"+ixn_hA->get_name()+"_"+pad_str(opt_step,4)+".txt");
			ixn_jAA->get_adjoint()->write_to_file("data_learned_visible/adjoint/"+ixn_jAA->get_name()+"_"+pad_str(opt_step,4)+".txt");
			ixn_kAAA->get_adjoint()->write_to_file("data_learned_visible/adjoint/"+ixn_kAAA->get_name()+"_"+pad_str(opt_step,4)+".txt");
			*/

		};

        // Write diff eq rhs
        if (opt_step == no_opt_steps) {
            rhs_hA->write_to_file(dir_write+"/diff_eq_rhs/"+ixn_hA->get_name()+"_"+pad_str(opt_step,4)+".txt");
            rhs_jAA->write_to_file(dir_write+"/diff_eq_rhs/"+ixn_jAA->get_name()+"_"+pad_str(opt_step,4)+".txt");
            rhs_kAAA->write_to_file(dir_write+"/diff_eq_rhs/"+ixn_kAAA->get_name()+"_"+pad_str(opt_step,4)+".txt");
        };
	};

	return 0;
};
