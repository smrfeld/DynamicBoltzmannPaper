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
void read_ixn_func_at_timepoint(string fname, std::shared_ptr<IxnParamTraj> ipt, int timepoint) {
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
        while (getline(f,line)) {
            if (line == "") { continue; };
            iss = std::istringstream(line);
            iss >> tstep;
            iss >> val;
            if (t==timepoint) {
                std::cout << ipt->get_name() << " " << val << std::endl;
                ipt->get_ixn_param_at_timepoint(0)->set_val(atof(val.c_str()));
                break; // Done!
            };
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
    init_conds["bX1"] = -1.3;
    init_conds["bY1"] = -1.3;
    init_conds["bZ1"] = -1.3;
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
    
    double spacing = 0.05;
    LatticeMode mode = LatticeMode::NORMAL;
    DBM dbm(1, spacing, init_conds, mode, lrs);
    
    // ***************
    // MARK: - Setup
    // ***************
    
    int no_timesteps_ixn_params = 1;
    
    int no_samples = 1;
    
    for (auto ixn: dbm.ixns) {
        ixn->set_no_timesteps(no_timesteps_ixn_params); // NOTE: MUST do this before dbm.latt->set_no_timesteps!
    };
    
    dbm.latt->set_no_markov_chains(MCType::ASLEEP, no_samples);
    dbm.latt->set_no_timesteps(0, no_timesteps_ixn_params);

    // ***************
    // MARK: - Read ixn params
    // ***************
    
    int timepoint_read = 20;
    
    int opt_step_read = 10000;
    
    for (auto ixn: dbm.ixns) {
        read_ixn_func_at_timepoint("../data/learn_sequential/ixn_params/"+ixn->get_name()+"_"+pad_str(opt_step_read,5)+".txt", ixn, timepoint_read);
    };

    // ***************
    // MARK: - Activate
    // ***************
    
    std::string dir = "../data/activate/";

     // Read an IC
    int i_read = 6;
    dbm.latt->get_lattice_at_timepoint(0)->read_layer_from_file(MCType::ASLEEP, 0, 0, "../../../rossler_stoch_sims/data_v1/lattice_v"+pad_str(i_read,3)+"/lattice/"+pad_str(timepoint_read,4)+".txt", true);
    
    // Activate the hiddens with prob
    dbm.latt->get_lattice_at_timepoint(0)->activate_layer_calculate_from_below(MCType::ASLEEP, 1);
    dbm.latt->get_lattice_at_timepoint(0)->activate_layer_convert_to_probs(MCType::ASLEEP, 1, false);
    dbm.latt->get_lattice_at_timepoint(0)->activate_layer_committ(MCType::ASLEEP, 1);
    
    // Write
    dbm.latt->get_lattice_at_timepoint(0)->write_layer_to_file(MCType::ASLEEP, 0, 1, dir+"/layer_1.txt",false);
    
	return 0;
};


