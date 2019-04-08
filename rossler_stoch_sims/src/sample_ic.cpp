#include <iostream>
#include <bmla>
#include <fstream>
#include <vector>

#include <sstream>

using namespace dblz;
using namespace std;

#include "dbm_v1.hpp"

int main() {

    // ***************
    // MARK: - DBM
    // ***************
  
    int no_hidden_layers = 2;
    DBM dbm(no_hidden_layers);

    // ***************
    // MARK: - set values
    // ***************
    
    for (auto ixn: dbm.ixns)
    {
        if (ixn->get_name() == "hA") {
            ixn->set_val(-2.04);
        } else if (ixn->get_name() == "hB") {
            ixn->set_val(0.62);
        } else if (ixn->get_name() == "hC") {
            ixn->set_val(-0.93);
        } else if (ixn->get_name() == "wAX1") {
            ixn->set_val(0.39);
        } else if (ixn->get_name() == "wBX1") {
            ixn->set_val(-0.58);
        } else if (ixn->get_name() == "wCX1") {
            ixn->set_val(0.12);
        } else if (ixn->get_name() == "bX1") {
            ixn->set_val(-0.34);
        } else if (ixn->get_name() == "wX1X2") {
            ixn->set_val(-0.68);
        } else if (ixn->get_name() == "bX2") {
            ixn->set_val(-1.03);
        };
    };
    
    // ***************
    // MARK: - Sample
    // ***************
    
    int no_samples = 100;
    dbm.latt->set_no_markov_chains(MCType::ASLEEP, no_samples);
    
    for (auto j=0; j<10; j++) {
        std::cout << "Sampling: " << j << std::endl;
        // Sample
        dbm.latt->gibbs_sampling_step(true, true);
    };
    
    std::string fname;
    for (auto i=1; i<=no_samples; i++) {
        std::cout << i << std::endl;
        
        fname = "../data/lattice_v" + pad_str(i,3) + "/lattice/0000.txt";

        // Write
        dbm.latt->write_layer_to_file(MCType::ASLEEP, i-1, 0, fname, true);
    };

	return 0;
};


