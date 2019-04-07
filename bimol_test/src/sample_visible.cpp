#include <iostream>
#include <dblz>
#include <fstream>
#include <vector>

#include <sstream>

using namespace dblz;
using namespace std;

// ***************
// MARK: - Read init conds
// ***************

vector<double> read_traj(string fname) {
	vector<double> ret;

	std::ifstream f;
	f.open(fname);
	std::string val="", time="";
	std::string line;
	std::istringstream iss;
	if (f.is_open()) { // make sure we found it
		while (getline(f,line)) {
			if (line == "") { continue; };
			iss = std::istringstream(line);
		    iss >> time >> val;
		    ret.push_back(atof(val.c_str()));
            val=""; time="";
		};
	};
	f.close();

	return ret;
};

// ***************
// MARK: - Write a vec
// ***************

void write_vec(vector<double> &vec, string fname) {
	std::ofstream f;
	f.open (fname);
	bool first_line=true;
	for (auto const &x: vec) {
		if (first_line) {
			first_line = false;
		} else {
			f << "\n";
		};
		// Write
		f << x;
	};
	f.close();
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
    
    auto ixn_hA = make_shared<IxnParam>("hA",IxnParamType::H, 0.0, 0.0);
    auto ixn_jAA = make_shared<IxnParam>("jAA",IxnParamType::J, 0.0, 0.0);
    auto ixn_kAAA = make_shared<IxnParam>("kAAA",IxnParamType::K, 0.0, 0.0);
    
    cout << "--- [Finished] Making ixn func ---" << endl;
    cout << endl;
    
    // ***************
    // MARK: - Setup lattice
    // ***************
    
    cout << "--- Making lattice ---" << endl;
    
    // 1000 cube
    bool conn_nn = true;
    bool conn_triplet = true;
    auto latt = make_shared<Lattice1DFullyVisible>(1000,sp_vis,conn_nn,conn_triplet);
    
    cout << " > begin visible" << endl;
    
    // Set bias
    latt->set_bias(species_A, ixn_hA);
    
    // NNs
    latt->set_ixn_2(species_A, species_A, ixn_jAA);
    
    // Triplets
    latt->set_ixn_3(species_A, species_A, species_A, ixn_kAAA);
    
    cout << " > end visible" << endl;

    // ***************
    // MARK: - Iterate over trajs
    // ***************
    
	// Params
	int no_latt_sampling_steps = 10;
	int no_samples_per_IC = 10;
    latt->set_no_markov_chains(MCType::AWAKE, no_samples_per_IC);
    latt->set_no_markov_chains(MCType::ASLEEP, no_samples_per_IC);

	vector<double> hA_vals, jAA_vals, kAAA_vals;
	vector<double> count_m1, count_m2, count_m3, count_m4;

	for (auto IC=1; IC<=50; IC++)
	{

		/********************
		Read trajs
		********************/

		hA_vals = read_traj("../data_learned_visible/trajs/hA_" + pad_str(IC,3) + ".txt");
		jAA_vals = read_traj("../data_learned_visible/trajs/jAA_" + pad_str(IC,3) + ".txt");
		kAAA_vals = read_traj("../data_learned_visible/trajs/kAAA_" + pad_str(IC,3) + ".txt");

		/********************
		Clear counts
		********************/

		count_m1.clear();
		count_m2.clear();
		count_m3.clear();
		count_m4.clear();

		/********************
		Loop over timesteps
		********************/

		for (auto timepoint=0; timepoint<hA_vals.size(); timepoint++) {

			/********************
			Set vals
			********************/

			ixn_hA->set_val(hA_vals[timepoint]);
			ixn_jAA->set_val(jAA_vals[timepoint]);
			ixn_kAAA->set_val(kAAA_vals[timepoint]);
            // std::cout << hA_vals[timepoint] << " " << jAA_vals[timepoint] << " " << kAAA_vals[timepoint] << std::endl;
            
			/********************
			Init counts
			********************/

			count_m1.push_back(0.0);
			count_m2.push_back(0.0);
			count_m3.push_back(0.0);
			count_m4.push_back(0.0);

            std::cout << IC << " " << timepoint << std::endl;

            /********************
            Clear lattice
            ********************/

            for (auto i_chain=0; i_chain<no_samples_per_IC; i_chain++) {
                latt->set_empty_all_units(MCType::AWAKE, i_chain);
                latt->set_empty_all_units(MCType::ASLEEP, i_chain);
            };

            /********************
            Sample lattice
            ********************/

            for (auto sampling_step=0; sampling_step<no_latt_sampling_steps; sampling_step++) {
                latt->gibbs_sampling_step();
            };

            /********************
             Reap moments over samples
             ********************/
            
            for (auto sample=0; sample<no_samples_per_IC; sample++) {

                /********************
                Get counts
                ********************/

                count_m1[timepoint] += latt->get_count(MCType::ASLEEP,sample,species_A);
                count_m2[timepoint] += latt->get_count(MCType::ASLEEP,sample,species_A,species_A);
                count_m3[timepoint] += latt->get_count(MCType::ASLEEP,sample,species_A,species_A,species_A);
                count_m4[timepoint] += latt->get_count(MCType::ASLEEP,sample,species_A,species_A,species_A,species_A);
			};

			/********************
			Average counts
			********************/

			count_m1[timepoint] /= no_samples_per_IC;
			count_m2[timepoint] /= no_samples_per_IC;
			count_m3[timepoint] /= no_samples_per_IC;
			count_m4[timepoint] /= no_samples_per_IC;
            // std::cout << count_m1[timepoint] << " " << count_m2[timepoint] << std::endl;

		};

		/********************
		Write counts
		********************/

		write_vec(count_m1,"../data_learned_visible/samples/ic_v" + pad_str(IC,3) + "_m1.txt");
		write_vec(count_m2,"../data_learned_visible/samples/ic_v" + pad_str(IC,3) + "_m2.txt");
		write_vec(count_m3,"../data_learned_visible/samples/ic_v" + pad_str(IC,3) + "_m3.txt");
		write_vec(count_m4,"../data_learned_visible/samples/ic_v" + pad_str(IC,3) + "_m4.txt");

	};

	return 0;
};


