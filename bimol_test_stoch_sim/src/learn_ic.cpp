#include <iostream>
#include <bmla>
#include <fstream>
#include <vector>

#include <sstream>

using namespace dblz;
using namespace std;

// ***************
// MARK: - Write function
// ***************

void write(double hA, double wAX, double bX, string fname) {
	std::ofstream f;

	// Open
	f.open(fname);

	// Make sure we found it
	if (!f.is_open()) {
		std::cerr << ">>> Error: write <<< could not write to file: " << fname << std::endl;
		exit(EXIT_FAILURE);
	};

	// Go through all time
	f << "hA " << hA << "\n";
	f << "wAX " << wAX << "\n";
	f << "bX " << bX;

	// Close
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
	auto species_X = make_shared<Species>("X");
    vector<shared_ptr<Species>> sp_vis({species_A});
    vector<shared_ptr<Species>> sp_hidden({species_X});

	cout << "--- [Finished] Making species ---" << endl;
	cout << endl;

    // ***************
    // MARK: - Make ixns
    // ***************

	cout << "--- Making ixn func ---" << endl;

    // Learning rate
    double dopt = 0.00001; // 0.00001
    
    // Initial guess
    double guess = 0.0;
    
    auto ixn_hA = make_shared<IxnParam>("hA",IxnParamType::H, guess, dopt);
    auto ixn_wAX = make_shared<IxnParam>("wAX",IxnParamType::W, guess, dopt);
    auto ixn_bX = make_shared<IxnParam>("bX",IxnParamType::B, guess, dopt);

	cout << "--- [Finished] Making ixn func ---" << endl;
	cout << endl;

    // ***************
    // MARK: - Setup lattice
    // ***************
    
    cout << "--- Making lattice ---" << endl;
    
    // 1000 cube
    auto latt = make_shared<Lattice>(1,1000,sp_vis,LatticeMode::NORMAL);
    
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
    OptProblemStatic opt(latt);

	// Params
	int no_opt_steps = 1000; // 1000
	int no_cd_steps = 1; // 1
    
    // Init batch size
    int batch_size = 10; // 10
    latt->set_no_markov_chains(MCType::AWAKE, batch_size);
    latt->set_no_markov_chains(MCType::ASLEEP, batch_size);

    // Solve options
    OptionsSolveStatic options;
    options.solver = Solver::SGD;
    
    // Wake sleep options
    OptionsWakeSleep_RBM_CD options_wake_sleep;

    // Directory to write to
    std::string dir_write = "../data_learn_ic/";

	/****************************************
	Iterate over IC
	****************************************/

	for (int iIC=1; iIC<=50; iIC++)
	{

		/********************
		Reset initial guess
		********************/

		/*
		ixn_hA->set_val(0.0);
		ixn_wAX->set_val(1.0);
		ixn_bX->set_val(-1.0);
		*/
		ixn_hA->set_val(0.0);
		ixn_wAX->set_val(0.0);
		ixn_bX->set_val(0.0);

		/********************
		Filenames
		********************/

		FNameColl fnames;
		for (auto i_batch=1; i_batch<=100; i_batch++) {
			// Add
            fnames.add_fname(FName("../data/ic_v" + pad_str(iIC,3) + "/lattice_v" + pad_str(i_batch,3) + "/lattice/0000.txt", true));
		};

		/********************
		Solve
		********************/

		double ixn_hA_ave=0.0, ixn_wAX_ave=0.0, ixn_bX_ave=0.0;
		int ave_window_size = 50;
		for (auto opt_step=1; opt_step<=no_opt_steps; opt_step++) {

			cout << "------------------" << endl;
			cout << "IC: " << iIC << "; Opt step: " << opt_step << " / " << no_opt_steps << endl;
			cout << "------------------" << endl;

            opt.solve_one_step_rbm_cd(opt_step, no_cd_steps, fnames, options, options_wake_sleep);

			// Params
			cout << ixn_hA->get_name() << " " << ixn_hA->get_val() << endl;
			cout << ixn_wAX->get_name() << " " << ixn_wAX->get_val() << endl;
			cout << ixn_bX->get_name() << " " << ixn_bX->get_val() << endl;

            /*
            // Write moments
            bool append = true;
            if (opt_step==1) {
                append = false;
            };
            ixn_hA->get_moment()->write_to_file(dir_write+"moments/"+ixn_hA->get_name()+".txt", append);
            ixn_wAX->get_moment()->write_to_file(dir_write+"moments/"+ixn_wAX->get_name()+".txt", append);
            ixn_bX->get_moment()->write_to_file(dir_write+"moments/"+ixn_bX->get_name()+".txt", append);
            
            // Write ixn params
            ixn_hA->write_to_file(dir_write+"ixn_params/"+ixn_hA->get_name()+".txt", append);
            ixn_wAX->write_to_file(dir_write+"ixn_params/"+ixn_wAX->get_name()+".txt", append);
            ixn_bX->write_to_file(dir_write+"ixn_params/"+ixn_bX->get_name()+".txt", append);
            */
            
			// Average last 100 pts
			if (no_opt_steps - opt_step < ave_window_size) {
				ixn_hA_ave += ixn_hA->get_val();
				ixn_wAX_ave += ixn_wAX->get_val();
				ixn_bX_ave += ixn_bX->get_val();
			};

		};

		// Average
		ixn_hA_ave /= ave_window_size;
		ixn_wAX_ave /= ave_window_size;
		ixn_bX_ave /= ave_window_size;

		// Write average
		write(ixn_hA_ave,ixn_wAX_ave,ixn_bX_ave,"../data/ic_v" + pad_str(iIC,3) + "/init_hidden.txt");
	};

	return 0;
};


