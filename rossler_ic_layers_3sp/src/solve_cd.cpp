#include <iostream>
#include <bmla>
#include <fstream>
#include <vector>
#include <map>

#include <sstream>

#include "dbm_3species.hpp"

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
    // MARK: - DBM
    // ***************
    
    LatticeMode mode = LatticeMode::NORMAL;
    int no_hidden_layers = 1;
    DBM dbm(no_hidden_layers,mode);

    // ***************
    // MARK: - Options
    // ***************
    
    OptionsSolveStatic options;
    OptionsWakeSleep_RBM_CD options_wake_sleep;
    
    options.verbose_moment = false;
    
    options.sliding_factor = 0.01;
    
    // Solver
    options.solver = Solver::SGD;
    
    // ***************
    // MARK: - Setup optimizer
    // ***************
    
    // No markov chains
    int no_markov_chains = 1;
    
    // Opt solver class
    OptProblemStatic opt(dbm.latt);

    // Set no markov chains
    dbm.latt->set_no_markov_chains(MCType::AWAKE, no_markov_chains);
    dbm.latt->set_no_markov_chains(MCType::ASLEEP, no_markov_chains);
    
    // Params
    int no_opt_steps = 5000; // 10000
    int no_cd_steps = 10;
    
    // ***************
    // MARK: - Learning rates
    // ***************
    
    for (auto ixn: dbm.bias.at(0)) {
        ixn.second->set_lr(0.0001); // 0.0001
    };
    if (no_hidden_layers >= 1) {
        for (auto ixn: dbm.bias.at(1)) {
            ixn.second->set_lr(0.0001); // 0.0001
        };
        for (auto ixn: dbm.w_ixns.at(0)) {
            ixn.second->set_lr(0.0001); // 0.0001
        };
    };
    if (no_hidden_layers >= 2) {
        for (auto ixn: dbm.bias.at(2)) {
            ixn.second->set_lr(0.00001);
        };
        for (auto ixn: dbm.w_ixns.at(1)) {
            ixn.second->set_lr(0.00001);
        };
    };
    
    // ***************
    // MARK: - Iterate over timepoints
    // ***************
    
    for (auto timepoint=0; timepoint<=190; timepoint+=10) {
        
        std::string dir = "../data/fine_tune_" + pad_str(dbm.no_hidden_layers,1) + "layers/"+pad_str(timepoint,4);
        
        // ***************
        // MARK: - Read initial guess
        // ***************
        
        // Read means
        double val = 0.0;
        auto init_means = read_means("../../rossler_stoch_sims/data_means/"+pad_str(timepoint+1,4)+".txt");
        for (auto ixn: dbm.ixns) {
            if (ixn->get_name() == "hA" || ixn->get_name() == "bX1") {
                val = log( init_means.at("A") / (1.0 - init_means.at("A") - init_means.at("B") - init_means.at("C")) );
                ixn->set_val(val);
                std::cout << "Set " << ixn->get_name() << " to " << val << std::endl;
            } else if (ixn->get_name() == "hB" || ixn->get_name() == "bY1") {
                val = log( init_means.at("B") / (1.0 - init_means.at("A") - init_means.at("B") - init_means.at("C")) );
                ixn->set_val(val);
                std::cout << "Set " << ixn->get_name() << " to " << val << std::endl;
            } else if (ixn->get_name() == "hC" || ixn->get_name() == "bZ1") {
                val = log( init_means.at("C") / (1.0 - init_means.at("A") - init_means.at("B") - init_means.at("C")) );
                ixn->set_val(val);
                std::cout << "Set " << ixn->get_name() << " to " << val << std::endl;
            };
        };
        
        // Other ixn params
        for (auto ixn: dbm.ixns) {
            if (ixn->get_name() == "wAX1" || ixn->get_name() == "wBY1" || ixn->get_name() == "wCZ1") {
                ixn->set_val(0.0);
            } else if (ixn->get_name() == "bX2" || ixn->get_name() == "bY2" || ixn->get_name() == "bZ2") {
                ixn->set_val(-1.95);
            };
        };
            
        // Centers
        /*
        dbm.latt->set_center_pt_for_species_in_layer(0, dbm.species_map.at(0).at("A"), init_means.at("A"));
        dbm.latt->set_center_pt_for_species_in_layer(0, dbm.species_map.at(0).at("B"), init_means.at("B"));
        dbm.latt->set_center_pt_for_species_in_layer(0, dbm.species_map.at(0).at("C"), init_means.at("C"));
        dbm.latt->set_center_pt_for_species_in_layer(1, dbm.species_map.at(1).at("X1"), 0.25);
        dbm.latt->set_center_pt_for_species_in_layer(1, dbm.species_map.at(1).at("Y1"), 0.25);
        dbm.latt->set_center_pt_for_species_in_layer(1, dbm.species_map.at(1).at("Z1"), 0.25);
         */
        
        // ***************
        // MARK: - Filenames
        // ***************
        
        FNameColl fnames;
        for (auto i_batch=1; i_batch<100; i_batch++) {
            // Add
            bool binary_read = true;
            fnames.add_fname(FName("../../rossler_stoch_sims/data/lattice_v" + pad_str(i_batch,3) + "/lattice/"+pad_str(timepoint,4)+".txt",binary_read));
        };
        
        // ***************
        // MARK: - Solve
        // ***************
        
        // means
        std::map<string,double> aves;
        for (auto ixn: dbm.ixns) {
            aves[ixn->get_name()] = 0.0;
        };
        int ave_window_size = 500; // 100
        for (auto opt_step=1; opt_step<=no_opt_steps; opt_step++) {
            
            cout << "------------------" << endl;
            cout << "Time: " << timepoint << " Opt step: " << opt_step << " / " << no_opt_steps << endl;
            cout << "------------------" << endl;
            
            // Solve
            opt.solve_one_step_rbm_cd(opt_step,no_cd_steps,fnames,options,options_wake_sleep);
            
            // Write moments
            if (opt_step % 10 == 1) {
                bool append = true;
                if (opt_step==1) {
                    append = false;
                };
                for (auto ixn: dbm.ixns) {
                    ixn->get_moment()->write_to_file(dir + "/moments/"+ixn->get_name()+".txt", append);
                };
                
                // Write ixn params
                for (auto ixn: dbm.ixns) {
                    ixn->write_to_file(dir + "/ixn_params/"+ixn->get_name()+".txt", append);
                };
            };
            
            // Write centers
            /*
            for (auto layer=0; layer<=no_hidden_layers; layer++) {
                if (modes.at(layer) == LayerMode::CENTERED || modes.at(layer) == LayerMode::CENTERED_M) {
                    dbm.latt->write_centers_to_file(layer,dir+"/centers/layer_"+pad_str(layer,1)+"_"+pad_str(opt_step,4)+".txt");
                } else if (modes.at(layer) == LayerMode::CENTERED_PT) {
                    dbm.latt->write_center_pt_to_file(layer,dir+"/center_pts/layer_"+pad_str(layer,1)+"_"+pad_str(opt_step,4)+".txt");
                };
            };
            */
            
            // Average last 100 pts
            if (no_opt_steps - opt_step < ave_window_size) {
                for (auto ixn: dbm.ixns) {
                    aves[ixn->get_name()] += ixn->get_val();
                };
            };
            
        };
        
        // Average
        for (auto ixn: dbm.ixns) {
            aves[ixn->get_name()] /= ave_window_size;
        };
        
        // Write final centers
        // dbm.latt->write_center_pts_to_file(dir+"/init_center.txt");

        // Write average
        write(aves,dir+"/init_hidden.txt");

    };
    
	return 0;
};

