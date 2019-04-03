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
    double spacing = 0.4;
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
    auto latt = make_shared<Lattice>(1,1000,sp_vis,LatticeMode::NORMAL);

	cout << " > begin visible" << endl;

	// NN, NNN
	latt->all_conns_vv_init();
	latt->all_conns_vvv_init();

	// Set possible species
	latt->all_units_v_add_possible_species(species_A);

	// Make bias dict
	auto bias_dict_visible = make_shared<BiasDict>();
	bias_dict_visible->add_ixn(species_A,ixn_hA);
	auto o2_dict_visible = make_shared<O2IxnDict>();
	o2_dict_visible->add_ixn(species_A,species_A,ixn_jAA);
	auto o3_dict_visible = make_shared<O3IxnDict>();
	o3_dict_visible->add_ixn(species_A,species_A,species_A,ixn_kAAA);

	// Add dict to latt
	latt->all_units_v_set_bias_dict(bias_dict_visible);
	latt->all_conns_vv_set_ixn_dict(o2_dict_visible);
	latt->all_conns_vvv_set_ixn_dict(o3_dict_visible);

	// Add all units to moment of ixn func
	latt->all_units_v_add_to_moment_h(ixn_hA->get_moment());
	latt->all_conns_vv_add_to_moment_j(ixn_jAA->get_moment());
	latt->all_conns_vvv_add_to_moment_k(ixn_kAAA->get_moment());

	cout << " > end visible" << endl;

	/****************************************
	Setup optimizer
	****************************************/

	// Opt solver class
	OptProblem opt(latt);

	// Add ixn params
	opt.add_ixn_param(ixn_hA);
	opt.add_ixn_param(ixn_kAAA);
	opt.add_ixn_param(ixn_jAA);

	// Params
	int no_opt_steps = 200-10;
	// int no_timesteps = 100;
	double dt=0.01;
	int no_latt_sampling_steps = 1;
	int batch_size = 10;
	double dopt=1.0;

	// Structures
	// opt.init_structures(no_timesteps,batch_size);

	// Options
	OptionsSolve options;
	options.VERBOSE_NU = false;
	options.VERBOSE_WAKE_ASLEEP = false;
	options.VERBOSE_MOMENT = false;
	options.MODE_random_integral_range = true;
	options.VAL_random_integral_range_size = 10;

	/********************
	Read init cond
	********************/

	// clock_t start = clock();    

	std::map<int,double> ixn_hA_IC, ixn_jAA_IC, ixn_kAAA_IC;
	for (auto IC=1; IC<=50; IC++) {
		auto init_cond_vector = read_init_conds("../bimol_train_stoch_sim/ic_v"+pad_str(IC,3)+"/init_visible.txt");

		ixn_hA_IC[IC] = init_cond_vector[0];
		ixn_jAA_IC[IC] = init_cond_vector[1];
		ixn_kAAA_IC[IC] = init_cond_vector[2];
	};

	// clock_t stop = clock();    
	// std::cout << (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC << std::endl;

	/********************
	Filenames
	********************/

	int no_timesteps_max = 100;

	std::map<int, FNameSeriesColl> fnames_map;
	for (auto IC=1; IC<=50; IC++) {
		fnames_map[IC] = FNameSeriesColl();

		for (auto i_batch=1; i_batch<=100; i_batch++) {

			// Series of filenames
			FNameSeries fname_series;
			for (auto timepoint=0; timepoint<=no_timesteps_max; timepoint++) {
				fname_series.fnames.push_back("../bimol_train_stoch_sim/ic_v"+pad_str(IC,3)+"/lattice_v"+pad_str(i_batch,3)+"/lattice/"+pad_str(timepoint,4)+".txt");
			};

			// Add
			fnames_map[IC].add_fname_series(fname_series);
		};
	};

	/********************
	Iterate over opt steps
	********************/

	// int IC = randI(1,20); // inclusive
	int no_timesteps = 5;
	options.VAL_random_integral_range_start = -5;
	options.VAL_random_integral_range_end = -5;

	// Opt step
	for (auto opt_step=1; opt_step<=no_opt_steps; opt_step++) {

		// Gradually increase window
		if (opt_step % 10 == 1) {
			no_timesteps += 5;
			opt.init_structures(no_timesteps,batch_size);

			// Endpt
			// options.VAL_random_integral_range_end = no_timesteps - options.VAL_random_integral_range_size;
			options.VAL_random_integral_range_start += 5;
			options.VAL_random_integral_range_end += 5;
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
			opt.solve_one_step(opt_step,no_timesteps,batch_size,dt,dopt,no_latt_sampling_steps,fnames_map[IC],options);

			// Sample moments (whole trajectory)
			if (opt_step==no_opt_steps) {
				opt.wake_sleep_loop(0,no_timesteps,batch_size,no_latt_sampling_steps,fnames_map[IC],false);
			};

			// Print moments
			std::cout << ixn_hA->get_name() << " " << std::flush;
			ixn_hA->get_moment()->print_moment_comparison();

			std::cout << ixn_jAA->get_name() << " " << std::flush;
			ixn_jAA->get_moment()->print_moment_comparison();

			std::cout << ixn_kAAA->get_name() << " " << std::flush;
			ixn_kAAA->get_moment()->print_moment_comparison();

			// Write moments
			ixn_hA->get_moment()->write_to_file("data_learned_visible/moments/"+ixn_hA->get_name()+"_"+pad_str(IC,3)+"_"+pad_str(opt_step,4)+".txt");
			ixn_jAA->get_moment()->write_to_file("data_learned_visible/moments/"+ixn_jAA->get_name()+"_"+pad_str(IC,3)+"_"+pad_str(opt_step,4)+".txt");
			ixn_kAAA->get_moment()->write_to_file("data_learned_visible/moments/"+ixn_kAAA->get_name()+"_"+pad_str(IC,3)+"_"+pad_str(opt_step,4)+".txt");

			// Write ixn params
			ixn_hA->write_to_file("data_learned_visible/ixn_params/"+ixn_hA->get_name()+"_"+pad_str(IC,3)+"_"+pad_str(opt_step,4)+".txt");
			ixn_jAA->write_to_file("data_learned_visible/ixn_params/"+ixn_jAA->get_name()+"_"+pad_str(IC,3)+"_"+pad_str(opt_step,4)+".txt");
			ixn_kAAA->write_to_file("data_learned_visible/ixn_params/"+ixn_kAAA->get_name()+"_"+pad_str(IC,3)+"_"+pad_str(opt_step,4)+".txt");

			// Write adjoint
			/*
			ixn_hA->get_adjoint()->write_to_file("data_learned_visible/adjoint/"+ixn_hA->get_name()+"_"+pad_str(opt_step,4)+".txt");
			ixn_jAA->get_adjoint()->write_to_file("data_learned_visible/adjoint/"+ixn_jAA->get_name()+"_"+pad_str(opt_step,4)+".txt");
			ixn_kAAA->get_adjoint()->write_to_file("data_learned_visible/adjoint/"+ixn_kAAA->get_name()+"_"+pad_str(opt_step,4)+".txt");
			*/

		};

		// Write diff eq rhs
		if (opt_step == no_opt_steps) {
			rhs_hA->write_to_file("data_learned_visible/diff_eq_rhs/"+ixn_hA->get_name()+"_"+pad_str(opt_step,4)+".txt");
			rhs_jAA->write_to_file("data_learned_visible/diff_eq_rhs/"+ixn_jAA->get_name()+"_"+pad_str(opt_step,4)+".txt");
			rhs_kAAA->write_to_file("data_learned_visible/diff_eq_rhs/"+ixn_kAAA->get_name()+"_"+pad_str(opt_step,4)+".txt");
		};
	};

	return 0;
};
