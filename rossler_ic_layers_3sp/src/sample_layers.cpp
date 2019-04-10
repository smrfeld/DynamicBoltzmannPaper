#include <iostream>
#include <bmla>
#include <fstream>
#include <vector>
#include <map>

#include <sstream>

#include "dbm_3species.hpp"

using namespace dblz;
using namespace std;

int main() {

    // ***************
    // MARK: - DBM
    // ***************
    
    LatticeMode mode = LatticeMode::NORMAL;
    int no_hidden_layers = 1;
    DBM dbm(no_hidden_layers,mode);
    
    // No samples
    int no_samples = 100;
    dbm.latt->set_no_markov_chains(MCType::ASLEEP, no_samples);
    
    // No sampling steps
    int no_gibbs_sampling_steps=10;
    
    // ***************
    // MARK: - Loop over time
    // ***************
    
    for (auto timepoint=130; timepoint<=130; timepoint += 10) {
        
        // ***************
        // MARK: - Read ixn params
        // **************
        
        for (auto ixn: dbm.ixns) {
            ixn->set_val(0.0);
        };
        dbm.read_init_conds_increment("../data/fine_tune_" + pad_str(dbm.no_hidden_layers,1) + "layers/" + pad_str(timepoint,4) + "/init_hidden.txt");

        // ***************
        // MARK: - Read centers
        // ***************
        
        // dbm.latt->read_centers_from_file(0, "../data/fine_tune_" + pad_str(dbm.no_hidden_layers,1) + "layers/" + pad_str(timepoint,4) + "/centers/layer_0_3000.txt");
        // dbm.latt->read_centers_from_file(1, "../data/fine_tune_" + pad_str(dbm.no_hidden_layers,1) + "layers/" + pad_str(timepoint,4) + "/centers/layer_1_3000.txt");
        
        /*
        dbm.latt->read_center_pt_from_file(0, "../data/fine_tune_" + pad_str(dbm.no_hidden_layers,1) + "layers/" + pad_str(timepoint,4) + "/center_pts/layer_0_2500.txt");
        dbm.latt->read_center_pt_from_file(1, "../data/fine_tune_" + pad_str(dbm.no_hidden_layers,1) + "layers/" + pad_str(timepoint,4) + "/center_pts/layer_1_2500.txt");
        dbm.latt->read_center_pt_from_file(2, "../data/fine_tune_" + pad_str(dbm.no_hidden_layers,1) + "layers/" + pad_str(timepoint,4) + "/center_pts/layer_2_2500.txt");
        dbm.latt->read_center_pt_from_file(3, "../data/fine_tune_" + pad_str(dbm.no_hidden_layers,1) + "layers/" + pad_str(timepoint,4) + "/center_pts/layer_3_2500.txt");
         */
        
        // dbm.latt->read_center_pts_from_file("../data/fine_tune_" + pad_str(dbm.no_hidden_layers,1) + "layers/" + pad_str(timepoint,4) + "/init_center.txt");
        
        // ***************
        // MARK: - Random initial lattice
        // ***************
        
        for (auto i_chain=0; i_chain<no_samples; i_chain++) {
            dbm.latt->set_random_all_units(MCType::ASLEEP, i_chain, true);
        };
        
        // ***************
        // MARK: - Read a real initial lattice
        // ***************
        
        /*
        FNameColl fnames;
        for (auto i_batch=1; i_batch<100; i_batch++) {
            fnames.add_fname(FName("../../rossler_stoch_sims/data/lattice_v" + pad_str(i_batch,3) + "/lattice/"+pad_str(timepoint,4)+".txt",true));
        };
        auto subset = fnames.get_random_subset_fnames(no_samples);
        for (int i_chain=0; i_chain<no_samples; i_chain++)
        {
            dbm.latt->read_layer_from_file(MCType::ASLEEP, i_chain, 0, subset[i_chain].name, subset[i_chain].binary);
        };
         */
        
        // ***************
        // MARK: - Sample
        // ***************
        
        // Up
        dbm.latt->activate_layer_calculate_from_below(MCType::ASLEEP, 1);
        dbm.latt->activate_layer_convert_to_probs(MCType::ASLEEP, 1, true);
        dbm.latt->activate_layer_committ(MCType::ASLEEP, 1);
        
        // Sample vis, hidden
        for (int i_sampling_step=0; i_sampling_step<no_gibbs_sampling_steps-1; i_sampling_step++)
        {
            std::cout << ".";
            
            // Down
            dbm.latt->activate_layer_calculate_from_above(MCType::ASLEEP, 0);
            dbm.latt->activate_layer_convert_to_probs(MCType::ASLEEP, 0, true);
            dbm.latt->activate_layer_committ(MCType::ASLEEP, 0);
            
            // Up
            dbm.latt->activate_layer_calculate_from_below(MCType::ASLEEP, 1);
            dbm.latt->activate_layer_convert_to_probs(MCType::ASLEEP, 1, true);
            dbm.latt->activate_layer_committ(MCType::ASLEEP, 1);
        };
        std::cout << std::endl;
        
        // Final step
        
        // Down
        dbm.latt->activate_layer_calculate_from_above(MCType::ASLEEP, 0);
        dbm.latt->activate_layer_convert_to_probs(MCType::ASLEEP, 0, true);
        dbm.latt->activate_layer_committ(MCType::ASLEEP, 0);

        // Up
        dbm.latt->activate_layer_calculate_from_below(MCType::ASLEEP, 1);
        dbm.latt->activate_layer_convert_to_probs(MCType::ASLEEP, 1, false);
        dbm.latt->activate_layer_committ(MCType::ASLEEP, 1);
        
        /********************
        Write
         ********************/
        
        // Dir to write to
        std::string dir = "../data/sample_" + pad_str(dbm.no_hidden_layers,1) + "layers/"+pad_str(timepoint,4);

        for (auto i_sample=0; i_sample<no_samples; i_sample++) {
            for (auto i_layer=0; i_layer<=no_hidden_layers; i_layer++) {
                if (i_layer == 0) {
                    dbm.latt->write_layer_to_file(MCType::ASLEEP, i_sample, i_layer, dir + "/layer_"+pad_str(i_layer,1)+"_"+pad_str(i_sample,4)+".txt", true);
                } else {
                    dbm.latt->write_layer_to_file(MCType::ASLEEP, i_sample, i_layer, dir + "/layer_"+pad_str(i_layer,1)+"_"+pad_str(i_sample,4)+".txt", false);
                };
            };
        };
        
    };
    
    return 0;
};

