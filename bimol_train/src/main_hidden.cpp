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
	auto species_X = make_shared<Species>("X");
    vector<shared_ptr<Species>> sp_vis({species_A});
    vector<shared_ptr<Species>> sp_hidden({species_X});

	cout << "--- [Finished] Making species ---" << endl;
	cout << endl;

    // ***************
    // MARK: - Make ixns
    // ***************

	cout << "--- Making ixn func ---" << endl;
    
    // Init cond
    double init_cond = 0.0;

	auto ixn_hA = make_shared<IxnParamTraj>("hA",IxnParamType::H, init_cond);
	auto ixn_wAX = make_shared<IxnParamTraj>("wAX",IxnParamType::W, init_cond);
	auto ixn_bX = make_shared<IxnParamTraj>("bX",IxnParamType::B, init_cond);

	cout << "--- [Finished] Making ixn func ---" << endl;
	cout << endl;
    
    // ***************
    // MARK: - Make RHS diff eqs
    // ***************

	cout << "--- Making RHS diff eq ---" << endl;

	// Domain
    double spacing_hA = 0.5;
    double spacing_wAX = 0.5;
    double spacing_bX = 0.05;
    double center = 0.0;
	Domain1D* domain_1d_hA = new Domain1D(ixn_hA,spacing_hA,center);
	Domain1D* domain_1d_wAX = new Domain1D(ixn_wAX,spacing_wAX,center);
	Domain1D* domain_1d_bX = new Domain1D(ixn_bX,spacing_bX,center);
	auto domain = std::vector<Domain1D*>({domain_1d_hA,domain_1d_wAX,domain_1d_bX});
    
    // Learning rate
    double dopt = 1.0;

	// RHS
	auto rhs_hA = make_shared<DiffEqRHS>("DE hA", ixn_hA, domain, dopt);
	ixn_hA->set_diff_eq_rhs(rhs_hA);
	auto rhs_wAX = make_shared<DiffEqRHS>("DE wAX", ixn_wAX, domain, dopt);
	ixn_wAX->set_diff_eq_rhs(rhs_wAX);
	auto rhs_bX = make_shared<DiffEqRHS>("DE bX", ixn_bX, domain, dopt);
	ixn_bX->set_diff_eq_rhs(rhs_bX);

    // Dependency
    ixn_hA->add_diff_eq_dependency(ixn_hA->get_diff_eq_rhs(), 0);
    ixn_hA->add_diff_eq_dependency(ixn_wAX->get_diff_eq_rhs(), 0);
    ixn_hA->add_diff_eq_dependency(ixn_bX->get_diff_eq_rhs(), 0);
    ixn_wAX->add_diff_eq_dependency(ixn_hA->get_diff_eq_rhs(), 1);
    ixn_wAX->add_diff_eq_dependency(ixn_wAX->get_diff_eq_rhs(), 1);
    ixn_wAX->add_diff_eq_dependency(ixn_bX->get_diff_eq_rhs(), 1);
    ixn_bX->add_diff_eq_dependency(ixn_hA->get_diff_eq_rhs(), 2);
    ixn_bX->add_diff_eq_dependency(ixn_wAX->get_diff_eq_rhs(), 2);
    ixn_bX->add_diff_eq_dependency(ixn_bX->get_diff_eq_rhs(), 2);

	cout << "--- [Finished] Making RHS diff eq ---" << endl;
	cout << endl;

    // ***************
    // MARK: - Adjoint
    // ***************
    
	cout << "--- Making adjoint ---" << endl;

	auto adjoint_hA = make_shared<Adjoint>("adjoint hA",ixn_hA);
	ixn_hA->set_adjoint(adjoint_hA);

	auto adjoint_wAX = make_shared<Adjoint>("adjoint wAX",ixn_wAX);
	ixn_wAX->set_adjoint(adjoint_wAX);

	auto adjoint_bX = make_shared<Adjoint>("adjoint bX",ixn_bX);
	ixn_bX->set_adjoint(adjoint_bX);

	cout << "--- [Finished] Making adjoint ---" << endl;
	cout << endl;

    // ***************
    // MARK: - Setup lattice
    // ***************
    
    cout << "--- Making lattice ---" << endl;
    
    // 1000 cube
    auto latt = make_shared<LatticeTraj>(1,1000,sp_vis,LatticeMode::NORMAL);
    
    cout << " > begin visible" << endl;
    
    // Add bias
    latt->set_bias_of_layer(0, species_A, ixn_hA);
    
    cout << " > end visible" << endl;
    
    cout << " > begin hidden" << endl;
    
    // Add layer
    // 999 units
    latt->add_layer(1, 999, sp_hidden);
    
    // Set bias
    latt->set_bias_of_layer(1, species_X, ixn_bX);
    
    // Connectivity hidden to visible
    for (auto hidden_unit=1; hidden_unit<1000; hidden_unit++) {
        latt->add_conn(0, hidden_unit, 1, hidden_unit);
        latt->add_conn(0, hidden_unit+1, 1, hidden_unit);
    }
    
    // Set ixn
    latt->set_ixn_between_layers(0, species_A, 1, species_X, ixn_wAX);
    
    cout << " > end hidden" << endl;
    
    cout << "--- [Finished] Making lattice ---" << endl;
    cout << endl;

    // ***************
    // MARK: - Setup optimizer
    // ***************
    
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
    OptionsWakeSleep_RBM_CD options_wake_sleep;
    options_wake_sleep.verbose_timing = false;
    
// Dir to write to
    std::string dir_write = "../data_learned_hidden";
    
    // No timesteps for ixn params
    int no_timesteps = 200;
    ixn_hA->set_no_timesteps(no_timesteps);
    ixn_wAX->set_no_timesteps(no_timesteps);
    ixn_bX->set_no_timesteps(no_timesteps);
    
    // No timesteps
    int timepoint_start_latt = 0;
    int no_timesteps_latt = 5;
    latt->set_no_timesteps(timepoint_start_latt, no_timesteps_latt);
    
    // ***************
    // MARK: - Read init cond
    // ***************
    
	std::map<int,double> ixn_hA_IC, ixn_wAX_IC, ixn_bX_IC;
	for (auto IC=1; IC<=50; IC++) {
		auto init_cond_vector = read_init_conds("../../bimol_train_stoch_sim/data/ic_v"+pad_str(IC,3)+"/init_hidden.txt");

		ixn_hA_IC[IC] = init_cond_vector[0];
		ixn_wAX_IC[IC] = init_cond_vector[1];
		ixn_bX_IC[IC] = init_cond_vector[2];
	};

    // ***************
    // MARK: - Filenames
    // ***************

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

    // ***************
    // MARK: - Solve
    // ***************

    int timepoint_start_solve_ixn_params = 0;
    int no_timesteps_solve_ixns_params = 5;
    
	for (auto opt_step=1; opt_step<=no_opt_steps; opt_step++) {

        // Slide window
        if (opt_step % 2 == 1 && opt_step != 1) {
            timepoint_start_latt += 1;
            no_timesteps_solve_ixns_params += 1;
            latt->set_no_timesteps(timepoint_start_latt, no_timesteps_latt);
		};
        
        // Iterate over ICs
		for (auto IC=1; IC<=50; IC++) {

			std::cout << "------------------------------------" << std::endl;
			std::cout << "Opt step: " << opt_step << " / " << no_opt_steps << "; init cond: " << IC << std::endl;
			std::cout << "------------------------------------" << std::endl;

			// Set init cond
			ixn_hA->set_init_cond(ixn_hA_IC[IC]);
			ixn_wAX->set_init_cond(ixn_wAX_IC[IC]);
			ixn_bX->set_init_cond(ixn_bX_IC[IC]);

            std::cout << ixn_hA_IC[IC] << " " << ixn_wAX_IC[IC] << " " << ixn_bX_IC[IC] << std::endl;
            
			// Take a step
            opt.solve_one_step_rbm_cd(opt_step, timepoint_start_solve_ixn_params, no_timesteps_solve_ixns_params, timepoint_start_latt, no_timesteps_latt, timepoint_start_latt, no_timesteps_latt, dt, no_cd_steps, fnames_map.at(IC), options, options_wake_sleep);

			// Sample moments (whole trajectory)
            /*
			if (opt_step==no_opt_steps) {
				opt.wake_sleep_loop(0,no_timesteps,batch_size,no_cd_steps,fnames_map[IC],false);
			};
             */
			// Write moments
            ixn_hA->write_moment_traj_to_file(timepoint_start_latt, no_timesteps_latt, dir_write+"/moments/"+ixn_hA->get_name()+"_"+pad_str(IC,3)+"_"+pad_str(opt_step,4)+".txt");
            ixn_wAX->write_moment_traj_to_file(timepoint_start_latt, no_timesteps_latt, dir_write+"/moments/"+ixn_wAX->get_name()+"_"+pad_str(IC,3)+"_"+pad_str(opt_step,4)+".txt");
            ixn_bX->write_moment_traj_to_file(timepoint_start_latt, no_timesteps_latt, dir_write+"/moments/"+ixn_bX->get_name()+"_"+pad_str(IC,3)+"_"+pad_str(opt_step,4)+".txt");

			// Write ixn params
            ixn_hA->write_val_traj_to_file(timepoint_start_latt, no_timesteps_latt, dir_write+"/ixn_params/"+ixn_hA->get_name()+"_"+pad_str(IC,3)+"_"+pad_str(opt_step,4)+".txt");
            ixn_wAX->write_val_traj_to_file(timepoint_start_latt, no_timesteps_latt, dir_write+"/ixn_params/"+ixn_wAX->get_name()+"_"+pad_str(IC,3)+"_"+pad_str(opt_step,4)+".txt");
            ixn_bX->write_val_traj_to_file(timepoint_start_latt, no_timesteps_latt, dir_write+"/ixn_params/"+ixn_bX->get_name()+"_"+pad_str(IC,3)+"_"+pad_str(opt_step,4)+".txt");

			// Write adjoint
			/*
			ixn_hA->get_adjoint()->write_to_file("data_learned_hidden/adjoint/"+ixn_hA->get_name()+"_"+pad_str(opt_step,4)+".txt");
			ixn_wAX->get_adjoint()->write_to_file("data_learned_hidden/adjoint/"+ixn_wAX->get_name()+"_"+pad_str(opt_step,4)+".txt");
			ixn_bX->get_adjoint()->write_to_file("data_learned_hidden/adjoint/"+ixn_bX->get_name()+"_"+pad_str(opt_step,4)+".txt");
			*/
		};

		// Write diff eq rhs
		if (opt_step == no_opt_steps) {
			rhs_hA->write_to_file(dir_write+"/diff_eq_rhs/"+ixn_hA->get_name()+"_"+pad_str(opt_step,4)+".txt");
			rhs_wAX->write_to_file(dir_write+"/diff_eq_rhs/"+ixn_wAX->get_name()+"_"+pad_str(opt_step,4)+".txt");
			rhs_bX->write_to_file(dir_write+"/diff_eq_rhs/"+ixn_bX->get_name()+"_"+pad_str(opt_step,4)+".txt");
		};
	};

	return 0;
};
