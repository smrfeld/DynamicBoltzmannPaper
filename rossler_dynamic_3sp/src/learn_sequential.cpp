#include <iostream>
#include <dblz>
#include <fstream>
#include <vector>
#include <map>

#include <sstream>

#include "dbm.hpp"

using namespace dblz;
using namespace std;

// ***************
// MARK: - Read means
// ***************

// Read init conds
map<string,double> read_means(string fname) {
    map<string,double> ret;
    
    std::ifstream f;
    f.open(fname);
    if (!f.is_open()) {
        std::cerr << ">>> read_means <<< could not find file: " << fname << std::endl;
        exit(EXIT_FAILURE);
    }
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
            ret[nu] = (atof(val.c_str())) / 1000.0;
            nu=""; val="";
        };
    };
    f.close();
    
    return ret;
};

int main() {

    // ***************
    // MARK: - Init conds
    // ***************
    
    int no_hidden_layers = 1;

    map<string,double> init_conds;
    // init_conds = read_init_conds("../../ic_layers_3species/data/fine_tune_" + pad_str(2,1) + "layers/" + pad_str(25,4) + "/init_hidden.txt");

    init_conds["hA"] = -0.69; // -0.69
    init_conds["hB"] = -0.69; // -0.69
    init_conds["hC"] = -0.69; // -0.69
    init_conds["wAX1"] = 0.0; // 0.0
    /*
    init_conds["wAY1"] = -0.1;
    init_conds["wAZ1"] = -0.1;
    init_conds["wBX1"] = -0.1;
     */
    init_conds["wBY1"] = 0.0; // 0.0
    /*
    init_conds["wBZ1"] = -0.1;
    init_conds["wCX1"] = -0.1;
    init_conds["wCY1"] = -0.1;
     */
    init_conds["wCZ1"] = 0.0; // 0.0
    init_conds["bX1"] = -1.95; // -1.95
    init_conds["bY1"] = -1.95; // -1.95
    init_conds["bZ1"] = -1.95; // -1.95
    /*
    init_conds["wX1X2"] = 0.0;
    init_conds["wY1Y2"] = 0.0;
    init_conds["wZ1Z2"] = 0.0;
    init_conds["bX2"] = -0.8;
    init_conds["bY2"] = -0.8;
    init_conds["bZ2"] = -0.8;
    */
    
    // ***************
    // MARK: - Learning rates
    // ***************
    
     // SGD
    double dopt_bias_0 = 0.8; // 0.1
    double dopt_bias_1 = 0.8; // 0.1
    double dopt_ixn_01 = 0.05; // 0.5
    // ADAM
    /*
    double dopt_bias_0 = 0.01; // 10
    double dopt_bias_1 = 0.01; // 10
    double dopt_ixn_01 = 0.01; // 10
     */
    map<string,double> lrs;
    lrs["hA"] = dopt_bias_0;
    lrs["hB"] = dopt_bias_0;
    lrs["hC"] = dopt_bias_0;
    lrs["wAX1"] = dopt_ixn_01;
    /*
    lrs["wAY1"] = dopt_ixn_01;
    lrs["wAZ1"] = dopt_ixn_01;
    lrs["wBX1"] = dopt_ixn_01;
     */
    lrs["wBY1"] = dopt_ixn_01;
    /*
    lrs["wBZ1"] = dopt_ixn_01;
    lrs["wCX1"] = dopt_ixn_01;
    lrs["wCY1"] = dopt_ixn_01;
     */
    lrs["wCZ1"] = dopt_ixn_01;
    lrs["bX1"] = dopt_bias_1;
    lrs["bY1"] = dopt_bias_1;
    lrs["bZ1"] = dopt_bias_1;
    /*
    lrs["wX1X2"] = dopt_ixn_12;
    lrs["wY1Y2"] = dopt_ixn_12;
    lrs["wZ1Z2"] = dopt_ixn_12;
    lrs["bX2"] = dopt_bias_2;
    lrs["bY2"] = dopt_bias_2;
    lrs["bZ2"] = dopt_bias_2;
     */
    
    // ***************
    // MARK: - DBM
    // ***************
    
    double spacing = 0.1; // 0.1
    // LatticeMode mode = LatticeMode::NORMAL_W_CENTERED_GRADIENT_PT;
    LatticeMode mode = LatticeMode::NORMAL;
    DBM dbm(no_hidden_layers, spacing, init_conds, mode, lrs);
    
    // ***************
    // MARK: - Read centers
    // ***************
    
    /*
    dbm.latt->get_lattice_at_timepoint(0)->set_center_vec_for_species_in_layer(0,dbm.species_map.at(0).at("A"),0.19);
    dbm.latt->get_lattice_at_timepoint(0)->set_center_vec_for_species_in_layer(0,dbm.species_map.at(0).at("B"),0.19);
    dbm.latt->get_lattice_at_timepoint(0)->set_center_vec_for_species_in_layer(0,dbm.species_map.at(0).at("C"),0.19);
    dbm.latt->get_lattice_at_timepoint(0)->set_center_vec_for_species_in_layer(1,dbm.species_map.at(1).at("X1"),0.19);
    dbm.latt->get_lattice_at_timepoint(0)->set_center_vec_for_species_in_layer(1,dbm.species_map.at(1).at("Y1"),0.19);
    dbm.latt->get_lattice_at_timepoint(0)->set_center_vec_for_species_in_layer(1,dbm.species_map.at(1).at("Z1"),0.19);
    dbm.latt->get_lattice_at_timepoint(0)->set_center_vec_for_species_in_layer(2,dbm.species_map.at(2).at("X2"),0.19);
    dbm.latt->get_lattice_at_timepoint(0)->set_center_vec_for_species_in_layer(2,dbm.species_map.at(2).at("Y2"),0.19);
    dbm.latt->get_lattice_at_timepoint(0)->set_center_vec_for_species_in_layer(2,dbm.species_map.at(2).at("Z2"),0.19);
    */
    
    /*
    dbm.latt->get_lattice_at_timepoint(0)->set_center_pt_for_species_in_layer(0,dbm.species_map.at(0).at("A"),0.19);
    dbm.latt->get_lattice_at_timepoint(0)->set_center_pt_for_species_in_layer(0,dbm.species_map.at(0).at("B"),0.19);
    dbm.latt->get_lattice_at_timepoint(0)->set_center_pt_for_species_in_layer(0,dbm.species_map.at(0).at("C"),0.19);
    dbm.latt->get_lattice_at_timepoint(0)->set_center_pt_for_species_in_layer(1,dbm.species_map.at(1).at("X1"),0.25);
    dbm.latt->get_lattice_at_timepoint(0)->set_center_pt_for_species_in_layer(1,dbm.species_map.at(1).at("Y1"),0.25);
    dbm.latt->get_lattice_at_timepoint(0)->set_center_pt_for_species_in_layer(1,dbm.species_map.at(1).at("Z1"),0.25);
     */
    
    /*
    dbm.latt->get_lattice_at_timepoint(0)->set_center_pt_for_species_in_layer(2,dbm.species_map.at(2).at("X2"),0.19);
    dbm.latt->get_lattice_at_timepoint(0)->set_center_pt_for_species_in_layer(2,dbm.species_map.at(2).at("Y2"),0.19);
    dbm.latt->get_lattice_at_timepoint(0)->set_center_pt_for_species_in_layer(2,dbm.species_map.at(2).at("Z2"),0.19);
     */
    
    // dbm.latt->get_lattice_at_timepoint(0)->read_center_pts_from_file("../../ic_layers_3species/data/fine_tune_" + pad_str(no_hidden_layers,1) + "layers/" + pad_str(25,4) + "/init_center.txt");

    // ***************
    // MARK: - Options
    // ***************
    
    OptionsSolveDynamic options;
    OptionsWakeSleep_RBM_CD options_wake_sleep;
    
    options.verbose = false;
    options.verbose_timing = false;
    
    options.no_steps_per_step_IP = 10;
    
    options.solver = Solver::SGD;
    // options.solver = Solver::ADAM;
    
    options.locking_mode = false;
    
    options_wake_sleep.verbose_timing = false;
    
    // ***************
    // MARK: - L2 reg
    // ***************

    int timepoint_max = 249;
    
    options.l2_reg = false;
    for (auto ixn: dbm.ixns) {
	if (ixn->get_type() == IxnParamType::H || ixn->get_type() == IxnParamType::B) {
        	options.l2_lambda[ixn] = 0.0;
        } else {
		options.l2_lambda[ixn] = 100.0;
	};
        options.l2_center[ixn] = init_conds.at(ixn->get_name());
    };
    
    /*
    double l2_mag = 100.0;    
    options.l2_reg_traj = true;
    std::map<string,double> means;
    for (auto t=0; t<timepoint_max; t++) {
        // Set center
        for (auto ixn: dbm.ixns) {
            options.l2_center_traj[ixn][t] = init_conds.at(ixn->get_name());
        };
        
        // Set mag
        means = read_means("../../../rossler_stoch_sims/data_means/" + pad_str(t,4) + ".txt");
        for (auto ixn: dbm.ixns) {
            if (ixn->get_name() == "wAX1" || ixn->get_name() == "hA" || ixn->get_name() == "bX1") {
                if (means.at("A") > 0.3) {
                    options.l2_lambda_traj[ixn][t] = 0.0;
                } else {
                    options.l2_lambda_traj[ixn][t] = l2_mag * (0.3 - means.at("A")) / 0.3;
                };
                // options.l2_lambda_traj[ixn][t] = 25.0;
            } else if (ixn->get_name() == "wBY1" || ixn->get_name() == "hB" || ixn->get_name() == "bY1") {
                if (means.at("B") > 0.3) {
                    options.l2_lambda_traj[ixn][t] = 0.0;
                } else {
                    options.l2_lambda_traj[ixn][t] = l2_mag * (0.3 - means.at("B")) / 0.3;
                };
                // options.l2_lambda_traj[ixn][t] = 25.0;              
            } else if (ixn->get_name() == "wCZ1" || ixn->get_name() == "hC" || ixn->get_name() == "bZ1") {
                if (means.at("C") > 0.3) {
                    options.l2_lambda_traj[ixn][t] = 0.0;
                } else {
                    options.l2_lambda_traj[ixn][t] = l2_mag * (0.3 - means.at("C")) / 0.3;
                };
                // options.l2_lambda_traj[ixn][t] = 25.0;
            } else {
                options.l2_lambda_traj[ixn][t] = 0.0;
            };
        };
    };
    */

    // ***************
    // MARK: - Setup optimizer
    // ***************
    
    int no_timesteps_ixn_params = 10;
    
    int timepoint_start_wake_sleep = 0;
    int no_timesteps_wake_sleep = 10;

    int timepoint_start_adjoint = 0;
    int no_timesteps_adjoint = 10;

    int no_markov_chains = 1;
    
	// Opt solver class
	OptProblemDynamic opt(dbm.latt);

    std::string dir = "../data/learn_sequential/";
    
    // ***************
    // MARK: - Filenames
    // ***************
    
    FNameTrajColl fnames;
    for (auto i_batch=1; i_batch<=300; i_batch++) {
        // Add
        bool binary_read = true;
        FNameTraj fname_traj;
        for (auto timepoint=0; timepoint<=timepoint_max; timepoint++) {
            fname_traj.push_back(FName("../../../rossler_stoch_sims/data/lattice_v" + pad_str(i_batch,3) + "/lattice/"+pad_str(timepoint,4)+".txt",binary_read));
        };
        
        fnames.add_fname_traj(fname_traj);
    };

    // ***************
    // MARK: - Set up the initial optimizer with the timepoints
    // ***************
    
    for (auto ixn: dbm.ixns) {
        ixn->set_no_timesteps(no_timesteps_ixn_params); // NOTE: MUST do this before dbm.latt->set_no_timesteps!
    };
    
    dbm.latt->set_no_markov_chains(MCType::AWAKE, no_markov_chains);
    dbm.latt->set_no_markov_chains(MCType::ASLEEP, no_markov_chains);
    dbm.latt->set_no_timesteps(timepoint_start_wake_sleep, no_timesteps_wake_sleep);

    // ***************
    // MARK: - Solve
    // ***************
    
    int no_opt_steps = 10000;
    double dt = 0.01;
    
    int no_cd_steps = 10; // 1
    
    // options_wake_sleep.sliding_factor = 0.01;

    int no_steps_move = 100; // 100
    
    // ***************
    // MARK: - Fix values
    // ***************
    
    /*
    for (auto ixn: dbm.ixns) {
        if (ixn->get_type() != IxnParamType::H) {
            ixn->set_fix_value(true);
        };
    };
     */
  
    // ***************
    // MARK: - Read the initial diff eqs
    // ***************
    
    /*
    for (auto ixn: dbm.ixns) {
        if (ixn->get_type() == IxnParamType::H) {
            ixn->get_diff_eq_rhs()->read_from_file("../data/learn_traj/diff_eq_rhs_dix/" + ixn->get_name() + "_0230.txt");
        };
    };
     */
    
    // ***************
    // MARK: - Solve
    // ***************
    
    for (auto opt_step=1; opt_step<=no_opt_steps; opt_step++) {

        cout << "------------------" << endl;
        cout << "Opt step: " << opt_step << " / " << no_opt_steps << endl;
        cout << "------------------" << endl;
        
        // TEMP: see if the oscillations stop
        /*
        if (opt_step == 1000) {
            for (auto ixn: dbm.ixns) {
                ixn->get_diff_eq_rhs()->set_lr(0.0);
            };
        };
         */
        
        // First learn biases
        /*  
        if (opt_step==1) {
            for (auto ixn: dbm.ixns) {
                if (ixn->get_type() == IxnParamType::H) {
                    ixn->set_fix_value(false);
                } else {
                    ixn->set_fix_value(true);
                };
            };
        } else if (opt_step == 100) {
            for (auto ixn: dbm.ixns) {
                if (ixn->get_type() == IxnParamType::H || ixn->get_name() == "wAX1" || ixn->get_name() == "wBY1"
                    || ixn->get_name() == "wCZ1" || ixn->get_name() == "bX1" || ixn->get_name() == "bY1"
                    || ixn->get_name() == "bZ1") {
                    ixn->set_fix_value(false);
                } else {
                    ixn->set_fix_value(true);
                };
            };
        } else if (opt_step == 300) {
            for (auto ixn: dbm.ixns) {
                ixn->set_fix_value(false);
            };
        };
        */
        
        // Fix centers for 0 timepoint
        // dbm.latt->get_lattice_at_timepoint(0)->read_center_pts_from_file("../../ic_layers/data/fine_tune_" + pad_str(no_hidden_layers,1) + "layers/" + pad_str(timepoint_start,4) + "/init_center.txt");
        
        // Move forward?
        if (opt_step > no_steps_move && opt_step % no_steps_move == 1) {
            
            // Ixn params
            no_timesteps_ixn_params += 1;
            for (auto ixn: dbm.ixns) {
                ixn->set_no_timesteps(no_timesteps_ixn_params);
            };
            
            // Move adjoint forward
            timepoint_start_adjoint += 1;
            
            // Fix the past
            /*
            for (auto ixn: dbm.ixns) {
                ixn->get_diff_eq_rhs()->fix_all_verts_around_at_timepoint(timepoint_start_adjoint-1, true);
            };
             */
            
            // If we want to see the moments over all times
            // no_timesteps_wake_sleep +=1;
            // Otherwise:
            timepoint_start_wake_sleep = timepoint_start_adjoint;
            
            dbm.latt->set_no_timesteps(0, no_timesteps_ixn_params);
        };
        
        // Batch size awake
// std::cout << opt_step << " " << 2*no_steps_move << " " << opt_step % 2*no_steps_move << std::endl;
        /*
        if (opt_step > 30*no_steps_move && opt_step % (30*no_steps_move) == 1) {
            
            // No awake markov chains
            no_markov_chains += 15;

            // Set
            dbm.latt->set_no_markov_chains(MCType::AWAKE, no_markov_chains);
            dbm.latt->set_no_markov_chains(MCType::ASLEEP, no_markov_chains);
        };

	std::cout << no_markov_chains << std::endl;
        */

        // Write out the means
        /*
        if (opt_step < 49) {
            for (auto layer=0; layer<=no_hidden_layers; layer++) {
                dbm.latt->get_lattice_at_timepoint(0)->write_centers_to_file(layer, dir+"centers_init/layer_"+pad_str(layer,1)+"_"+pad_str(opt_step,4)+".txt");
            };
        };
        */
        
        // Solve
        if (opt_step % 500 == 0) {
                // Sample more
                dbm.latt->set_no_timesteps(0, no_timesteps_ixn_params);
              opt.solve_one_step_rbm_cd(opt_step, 0, no_timesteps_ixn_params, 0, no_timesteps_ixn_params, timepoint_start_adjoint, no_timesteps_adjoint, dt, no_cd_steps, fnames, options, options_wake_sleep);
        } else {
                opt.solve_one_step_rbm_cd(opt_step, 0, no_timesteps_ixn_params, timepoint_start_wake_sleep, no_timesteps_wake_sleep, timepoint_start_adjoint, no_timesteps_adjoint, dt, no_cd_steps, fnames, options, options_wake_sleep);
        };

        // Write
        if (opt_step == 1 || opt_step % 500 == 0) {
            int i_write = opt_step;
            if (opt_step == 1) {
                i_write = 0;
            };
            
            /// Moments
            for (auto ixn: dbm.ixns) {
                ixn->write_moment_traj_to_file(0, no_timesteps_ixn_params, dir + "moments/" + ixn->get_name() + "_" + pad_str(i_write,5) + ".txt");
            };
            
            // Ixns
            for (auto ixn: dbm.ixns) {
                ixn->write_val_traj_to_file(0, no_timesteps_ixn_params, dir + "ixn_params/" + ixn->get_name() + "_" + pad_str(i_write,5) + ".txt");
            };
            
            // Centers
            /*
            for (auto timepoint=timepoint_start_wake_sleep; timepoint<=timepoint_start_wake_sleep+no_timesteps_wake_sleep; timepoint++) {
                dbm.latt->get_lattice_at_timepoint(timepoint)->write_center_pts_to_file(dir + "center_pts/" + pad_str(opt_step,4) + "_" + pad_str(timepoint,3) + ".txt");
            };
             */
            
            /*
            // Adjoints
            for (auto ixn: dbm.ixns) {
                ixn->write_adjoint_traj_to_file(0, no_timesteps_ixn_params, dir + "adjoints/" + ixn->get_name() + "_" + pad_str(opt_step-1,4) + ".txt");
            };
             */

            // Diff eqs
            for (auto ixn: dbm.ixns) {
                ixn->get_diff_eq_rhs()->write_to_file(dir + "diff_eq_rhs/" + ixn->get_name() + "_" + pad_str(i_write,5) + ".txt");
            };

                // Reset
                dbm.latt->set_no_timesteps(timepoint_start_wake_sleep, no_timesteps_wake_sleep);

        };
    };
    
	return 0;
};


