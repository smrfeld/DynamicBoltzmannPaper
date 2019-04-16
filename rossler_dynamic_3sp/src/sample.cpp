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

// ***************
// MARK: - Read ixn func
// ***************

// Read init conds
void read_ixn_func_traj(string fname, std::shared_ptr<IxnParamTraj> ipt, int no_timesteps) {
    std::ifstream f;
    f.open(fname);
    if (!f.is_open()) {
        std::cerr << ">>> read_ixn_func_traj <<< could not find file: " << fname << std::endl;
        exit(EXIT_FAILURE);
    }
    std::string tstep="";
    std::string val="";
    std::string line;
    std::istringstream iss;
    int t=0;
    if (f.is_open()) { // make sure we found it
        while (getline(f,line) && t<no_timesteps) {
            if (line == "") { continue; };
            iss = std::istringstream(line);
            iss >> tstep;
            iss >> val;
            ipt->get_ixn_param_at_timepoint(t)->set_val(atof(val.c_str()));
            tstep=""; val="";
            t++;
        };
    };
    f.close();
};

int main() {

    // ***************
    // MARK: - Init conds
    // ***************
    
    map<string,double> init_conds;
    init_conds["hA"] = -0.69;
    init_conds["hB"] = -0.69;
    init_conds["hC"] = -0.69;
    init_conds["wAX1"] = 0.0;
    /*
     init_conds["wAY1"] = -0.1;
     init_conds["wAZ1"] = -0.1;
     init_conds["wBX1"] = -0.1;
     */
    init_conds["wBY1"] = 0.0;
    /*
     init_conds["wBZ1"] = -0.1;
     init_conds["wCX1"] = -0.1;
     init_conds["wCY1"] = -0.1;
     */
    init_conds["wCZ1"] = 0.0;
    init_conds["bX1"] = -1.95;
    init_conds["bY1"] = -1.95;
    init_conds["bZ1"] = -1.95;
    /*
     init_conds["wX1X2"] = 0.0;
     init_conds["wY1Y2"] = 0.0;
     init_conds["wZ1Z2"] = 0.0;
     init_conds["bX2"] = -0.8;
     init_conds["bY2"] = -0.8;
     init_conds["bZ2"] = -0.8;
     */

    // ***************
    // MARK: - Learning rates - DOESNT MATTER
    // ***************
    
    map<string,double> lrs;
    lrs["hA"] = 0.0;
    lrs["hB"] = 0.0;
    lrs["hC"] = 0.0;
    lrs["wAX1"] = 0.0;
    /*
    lrs["wAY1"] = 0.0;
    lrs["wAZ1"] = 0.0;
    lrs["wBX1"] = 0.0;
     */
    lrs["wBY1"] = 0.0;
    /*
    lrs["wBZ1"] = 0.0;
    lrs["wCX1"] = 0.0;
    lrs["wCY1"] = 0.0;
     */
    lrs["wCZ1"] = 0.0;
    lrs["bX1"] = 0.0;
    lrs["bY1"] = 0.0;
    lrs["bZ1"] = 0.0;
    
    // ***************
    // MARK: - DBM
    // ***************
    
    double spacing = 0.01;
    LatticeMode mode = LatticeMode::NORMAL;
    DBM dbm(1, spacing, init_conds, mode, lrs);

    // ***************
    // MARK: - Setup optimizer
    // ***************
    
    int no_timesteps_ixn_params = 100;
    
    int opt_step_read = 10000;
    
    int no_samples = 100;
    
    // ***************
    // MARK: - Set up the initial optimizer with the timepoints
    // ***************
    
    for (auto ixn: dbm.ixns) {
        ixn->set_no_timesteps(no_timesteps_ixn_params); // NOTE: MUST do this before dbm.latt->set_no_timesteps!
    };
    
    dbm.latt->set_no_markov_chains(MCType::ASLEEP, no_samples);
    dbm.latt->set_no_timesteps(0, no_timesteps_ixn_params);

    // ***************
    // MARK: - Read & Solve diff eqs
    // ***************
    
    /*
    // Read
    int opt_step_read = 6000;
    for (auto ixn: dbm.ixns) {
        ixn->get_diff_eq_rhs()->read_from_file("../data/learn_sequential/diff_eq_rhs/"+ixn->get_name()+"_"+pad_str(opt_step_read,5)+".txt");
    };
    
    // Solve
    double dt = 0.01;
    int no_substeps = 10;
    OptProblemDynamic opt(dbm.latt);
    opt.solve_ixn_param_trajs(dt, 0, no_timesteps_ixn_params, no_substeps);
    
    // Write ixn params
    string dir_ip = "../data/sample_ixn_params";
    for (auto ixn: dbm.ixns) {
        ixn->write_val_traj_to_file(0, no_timesteps_ixn_params, dir_ip+"/"+ixn->get_name()+".txt");
    };
     */
    
    // ***************
    // MARK: - Read ixn param traj
    // ***************
    
    for (auto ixn: dbm.ixns) {
        read_ixn_func_traj("../data/learn_sequential/ixn_params/"+ixn->get_name()+"_"+pad_str(opt_step_read,5)+".txt", ixn, no_timesteps_ixn_params);
    };

    // ***************
    // MARK: - Sample
    // ***************
    
    int no_gibbs_sampling_steps = 10;
    std::string dir = "../data/sample/";

    for (auto timepoint=0; timepoint<=no_timesteps_ixn_params; timepoint++) {

        // Start from random
        for (auto i_chain=0; i_chain<no_samples; i_chain++) {
            dbm.latt->get_lattice_at_timepoint(timepoint)->set_random_all_units(MCType::ASLEEP, i_chain, true);
        };
        
         // Read an IC
        /*
        for (int i_chain=0; i_chain<no_samples; i_chain++)
        {
            dbm.latt->get_lattice_at_timepoint(timepoint)->read_layer_from_file(MCType::ASLEEP, i_chain, 0, "../../../rossler_stoch_sims/data_v1/lattice_v"+pad_str(i_chain+1,3)+"/lattice/"+pad_str(timepoint,4)+".txt", true);
        };
        */
        
        // Activate the hiddens with binary
        dbm.latt->get_lattice_at_timepoint(timepoint)->activate_layer_calculate_from_below(MCType::ASLEEP, 1);
        dbm.latt->get_lattice_at_timepoint(timepoint)->activate_layer_convert_to_probs(MCType::ASLEEP, 1, true);
        dbm.latt->get_lattice_at_timepoint(timepoint)->activate_layer_committ(MCType::ASLEEP, 1);
        
        // Sample vis, hidden repeadedly
        for (int i_sampling_step=0; i_sampling_step<no_gibbs_sampling_steps-1; i_sampling_step++)
        {
            std::cout << "time: " << timepoint << " step: " << i_sampling_step << std::endl;
            
            dbm.latt->get_lattice_at_timepoint(timepoint)->activate_layer_calculate_from_above(MCType::ASLEEP, 0);
            dbm.latt->get_lattice_at_timepoint(timepoint)->activate_layer_convert_to_probs(MCType::ASLEEP, 0, true);
            dbm.latt->get_lattice_at_timepoint(timepoint)->activate_layer_committ(MCType::ASLEEP, 0);
            
            dbm.latt->get_lattice_at_timepoint(timepoint)->activate_layer_calculate_from_below(MCType::ASLEEP, 1);
            dbm.latt->get_lattice_at_timepoint(timepoint)->activate_layer_convert_to_probs(MCType::ASLEEP, 1, true);
            dbm.latt->get_lattice_at_timepoint(timepoint)->activate_layer_committ(MCType::ASLEEP, 1);
        };
        // Final step: use probs for hidden layer
        dbm.latt->get_lattice_at_timepoint(timepoint)->activate_layer_calculate_from_above(MCType::ASLEEP, 0);
        dbm.latt->get_lattice_at_timepoint(timepoint)->activate_layer_convert_to_probs(MCType::ASLEEP, 0, true);
        dbm.latt->get_lattice_at_timepoint(timepoint)->activate_layer_committ(MCType::ASLEEP, 0);
        
        dbm.latt->get_lattice_at_timepoint(timepoint)->activate_layer_calculate_from_below(MCType::ASLEEP, 1);
        dbm.latt->get_lattice_at_timepoint(timepoint)->activate_layer_convert_to_probs(MCType::ASLEEP, 1, false);
        dbm.latt->get_lattice_at_timepoint(timepoint)->activate_layer_committ(MCType::ASLEEP, 1);
        
        /*
        for (auto i=0; i<no_gibbs_sampling_steps-1; i++) {
            std::cout << "time: " << timepoint << " step: " << i << std::endl;
            dbm.latt->get_lattice_at_timepoint(timepoint)->gibbs_sampling_step(true, true);
        };
        dbm.latt->get_lattice_at_timepoint(timepoint)->gibbs_sampling_step_parallel(false, false);
         */
        
        // Write
        for (auto i=0; i<no_samples; i++) {
            dbm.latt->get_lattice_at_timepoint(timepoint)->write_layer_to_file(MCType::ASLEEP, i, 0, dir+pad_str(timepoint,4)+"/layer_0_"+pad_str(i,4)+".txt",true);
            dbm.latt->get_lattice_at_timepoint(timepoint)->write_layer_to_file(MCType::ASLEEP, i, 1, dir+pad_str(timepoint,4)+"/layer_1_"+pad_str(i,4)+".txt",false);
        };
    };
    
	return 0;
};


