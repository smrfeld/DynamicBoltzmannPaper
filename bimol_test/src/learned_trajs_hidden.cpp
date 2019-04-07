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
    // MARK: - Read in
    // ***************
    
    int idx_read = 190;
    rhs_hA->read_from_file("../../bimol_train/data_learned_hidden/diff_eq_rhs/hA_"+pad_str(idx_read,4)+".txt");
    rhs_wAX->read_from_file("../../bimol_train/data_learned_hidden/diff_eq_rhs/wAX_"+pad_str(idx_read,4)+".txt");
    rhs_bX->read_from_file("../../bimol_train/data_learned_hidden/diff_eq_rhs/bX_"+pad_str(idx_read,4)+".txt");
    
    // ***************
    // MARK: - Go through all IC
    // ***************
    
    int no_timesteps = 200;
    double dt=0.01;
    
    // Init structure
    ixn_hA->set_no_timesteps(no_timesteps);
    ixn_wAX->set_no_timesteps(no_timesteps);
    ixn_bX->set_no_timesteps(no_timesteps);
    
    for (int IC=1; IC<=50; IC++) {
        
        std::cout << "IC: " << IC << std::endl;
        
        /********************
         Read init cond
         ********************/
        
        auto init_cond_vector = read_init_conds("../../bimol_test_stoch_sim/data/ic_v"+pad_str(IC,3)+"/init_hidden.txt");
        
        // Set
        ixn_hA->set_init_cond(init_cond_vector[0]);
        ixn_wAX->set_init_cond(init_cond_vector[1]);
        ixn_bX->set_init_cond(init_cond_vector[2]);
        
        /********************
         Solve
         ********************/
        
        for (auto t=0; t<no_timesteps; t++) {
            ixn_hA->solve_diff_eq_at_timepoint_to_plus_one(t,dt);
            ixn_wAX->solve_diff_eq_at_timepoint_to_plus_one(t,dt);
            ixn_bX->solve_diff_eq_at_timepoint_to_plus_one(t,dt);
        };
        
        /********************
         Write
         ********************/
        
        ixn_hA->write_val_traj_to_file(0, no_timesteps, "../data_learned_hidden/trajs/"+ixn_hA->get_name()+"_"+pad_str(IC,3)+".txt");
        ixn_wAX->write_val_traj_to_file(0, no_timesteps, "../data_learned_hidden/trajs/"+ixn_wAX->get_name()+"_"+pad_str(IC,3)+".txt");
        ixn_bX->write_val_traj_to_file(0, no_timesteps, "../data_learned_hidden/trajs/"+ixn_bX->get_name()+"_"+pad_str(IC,3)+".txt");
    };
    
    return 0;
};
