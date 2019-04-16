#include <iostream>
#include <dblz>
#include <fstream>
#include <vector>
#include <map>

#include <sstream>

using namespace dblz;
using namespace std;

// ***************
// MARK: - Read init conds
// ***************

// Read init conds
map<string,double> read_init_conds(string fname) {
    map<string,double> ret;
    
    std::ifstream f;
    f.open(fname);
    if (!f.is_open()) {
        std::cerr << ">>> read_init_conds <<< could not find file: " << fname << std::endl;
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
            ret[nu] = (atof(val.c_str()));
            // std::cout << "read_init_conds: read: " << nu << " " << ret.at(nu) << std::endl;
            nu=""; val="";
        };
    };
    f.close();
    
    return ret;
};

// ***************
// MARK: - DBM
// ***************

struct DBM {
    
    // ***************
    // MARK: - Members
    // ***************
    
    // Mode
    LatticeMode mode;
    
    // No hidden layers
    int no_hidden_layers;
    
    // Species
    map<int,map<string,shared_ptr<Species>>> species_map;
    map<int,vector<shared_ptr<Species>>> species_vec;
    
    // Ixn funcs
    map<int,map<string,ITptr>> bias;
    map<int,map<string,ITptr>> w_ixns;
    vector<ITptr> ixns;
    
    // Lattice
    shared_ptr<LatticeTraj> latt;
    
    // Init conds
    map<string, double> init_conds;
    
    // Domain
    /*
    std::vector<Domain1D*> domB0;
    std::vector<Domain1D*> domB1;
    std::vector<Domain1D*> domB2;
     */
    /*
    std::vector<Domain1D*> domA;
    std::vector<Domain1D*> domB;
    std::vector<Domain1D*> domC;
     */
    // std::vector<Domain1D*> domB0, domB1, domW0;
    std::vector<Domain1D*> dom;
    
    // ***************
    // MARK: - Constructor
    // ***************

    DBM(int no_hidden_layers, double spacing, map<string, double> init_conds, LatticeMode mode, std::map<string,double> lrs) {
        
        this->no_hidden_layers = no_hidden_layers;
        this->init_conds = init_conds;
        this->mode = mode;
        
        // ***************
        // MARK: - Species
        // ***************
        
        cout << "--- Making species ---" << endl;
        
        string sp_name;
        
        // 0
        int layer_v = 0;
        sp_name = "A";
        species_map[layer_v][sp_name] = make_shared<Species>(sp_name);
        species_vec[layer_v].push_back(species_map[layer_v][sp_name]);
        sp_name = "B";
        species_map[layer_v][sp_name] = make_shared<Species>(sp_name);
        species_vec[layer_v].push_back(species_map[layer_v][sp_name]);
        sp_name = "C";
        species_map[layer_v][sp_name] = make_shared<Species>(sp_name);
        species_vec[layer_v].push_back(species_map[layer_v][sp_name]);
            
        for (auto layer_h=1; layer_h<=no_hidden_layers; layer_h++) {
            sp_name = "X" + pad_str(layer_h,1);
            species_map[layer_h][sp_name] = make_shared<Species>(sp_name);
            species_vec[layer_h].push_back(species_map[layer_h][sp_name]);

            sp_name = "Y" + pad_str(layer_h,1);
            species_map[layer_h][sp_name] = make_shared<Species>(sp_name);
            species_vec[layer_h].push_back(species_map[layer_h][sp_name]);

            sp_name = "Z" + pad_str(layer_h,1);
            species_map[layer_h][sp_name] = make_shared<Species>(sp_name);
            species_vec[layer_h].push_back(species_map[layer_h][sp_name]);
        };
        
        cout << "--- [Finished] Making species ---" << endl;
        cout << endl;
        
        // ***************
        // MARK: - Ixn funcs
        // ***************
        
        cout << "--- Making ixn func ---" << endl;
        
        std::string ixn_name;
        
        // Layer 0
        // Ws
        ixn_name = "wAX1";
        w_ixns[layer_v][ixn_name] = make_shared<IxnParamTraj>(ixn_name,IxnParamType::W, init_conds.at(ixn_name));
        ixns.push_back(w_ixns[layer_v][ixn_name]);
        
        /*
        ixn_name = "wAY1";
        w_ixns[layer_v][ixn_name] = make_shared<IxnParamTraj>(ixn_name,IxnParamType::W, init_conds.at(ixn_name));
        ixns.push_back(w_ixns[layer_v][ixn_name]);
        
        ixn_name = "wAZ1";
        w_ixns[layer_v][ixn_name] = make_shared<IxnParamTraj>(ixn_name,IxnParamType::W, init_conds.at(ixn_name));
        ixns.push_back(w_ixns[layer_v][ixn_name]);
        
        ixn_name = "wBX1";
        w_ixns[layer_v][ixn_name] = make_shared<IxnParamTraj>(ixn_name,IxnParamType::W, init_conds.at(ixn_name));
        ixns.push_back(w_ixns[layer_v][ixn_name]);
        */
        
        ixn_name = "wBY1";
        w_ixns[layer_v][ixn_name] = make_shared<IxnParamTraj>(ixn_name,IxnParamType::W, init_conds.at(ixn_name));
        ixns.push_back(w_ixns[layer_v][ixn_name]);

        /*
        ixn_name = "wBZ1";
        w_ixns[layer_v][ixn_name] = make_shared<IxnParamTraj>(ixn_name,IxnParamType::W, init_conds.at(ixn_name));
        ixns.push_back(w_ixns[layer_v][ixn_name]);
        
        ixn_name = "wCX1";
        w_ixns[layer_v][ixn_name] = make_shared<IxnParamTraj>(ixn_name,IxnParamType::W, init_conds.at(ixn_name));
        ixns.push_back(w_ixns[layer_v][ixn_name]);
        
        ixn_name = "wCY1";
        w_ixns[layer_v][ixn_name] = make_shared<IxnParamTraj>(ixn_name,IxnParamType::W, init_conds.at(ixn_name));
        ixns.push_back(w_ixns[layer_v][ixn_name]);
         */
        
        ixn_name = "wCZ1";
        w_ixns[layer_v][ixn_name] = make_shared<IxnParamTraj>(ixn_name,IxnParamType::W, init_conds.at(ixn_name));
        ixns.push_back(w_ixns[layer_v][ixn_name]);

        // Bias visibles
        ixn_name = "hA";
        bias[layer_v][ixn_name] = make_shared<IxnParamTraj>(ixn_name,IxnParamType::H, init_conds.at(ixn_name));
        ixns.push_back(bias[layer_v][ixn_name]);
        
        ixn_name = "hB";
        bias[layer_v][ixn_name] = make_shared<IxnParamTraj>(ixn_name,IxnParamType::H, init_conds.at(ixn_name));
        ixns.push_back(bias[layer_v][ixn_name]);
        
        ixn_name = "hC";
        bias[layer_v][ixn_name] = make_shared<IxnParamTraj>(ixn_name,IxnParamType::H, init_conds.at(ixn_name));
        ixns.push_back(bias[layer_v][ixn_name]);

        // Hidden layers
        for (auto layer_h=1; layer_h<=no_hidden_layers; layer_h++) {

            // Ws
            if (layer_h != no_hidden_layers) {
                ixn_name = "wX" + pad_str(layer_h,1) + "X" + pad_str(layer_h+1,1);
                w_ixns[layer_h][ixn_name] = make_shared<IxnParamTraj>(ixn_name,IxnParamType::W, init_conds.at(ixn_name));
                ixns.push_back(w_ixns[layer_h][ixn_name]);

                ixn_name = "wY" + pad_str(layer_h,1) + "Y" + pad_str(layer_h+1,1);
                w_ixns[layer_h][ixn_name] = make_shared<IxnParamTraj>(ixn_name,IxnParamType::W, init_conds.at(ixn_name));
                ixns.push_back(w_ixns[layer_h][ixn_name]);

                ixn_name = "wZ" + pad_str(layer_h,1) + "Z" + pad_str(layer_h+1,1);
                w_ixns[layer_h][ixn_name] = make_shared<IxnParamTraj>(ixn_name,IxnParamType::W, init_conds.at(ixn_name));
                ixns.push_back(w_ixns[layer_h][ixn_name]);
            };
            
            // Bias hidden
            ixn_name = "bX" + pad_str(layer_h,1);
            bias[layer_h][ixn_name] = make_shared<IxnParamTraj>(ixn_name,IxnParamType::B, init_conds.at(ixn_name));
            ixns.push_back(bias[layer_h][ixn_name]);

            ixn_name = "bY" + pad_str(layer_h,1);
            bias[layer_h][ixn_name] = make_shared<IxnParamTraj>(ixn_name,IxnParamType::B, init_conds.at(ixn_name));
            ixns.push_back(bias[layer_h][ixn_name]);

            ixn_name = "bZ" + pad_str(layer_h,1);
            bias[layer_h][ixn_name] = make_shared<IxnParamTraj>(ixn_name,IxnParamType::B, init_conds.at(ixn_name));
            ixns.push_back(bias[layer_h][ixn_name]);
        };
        
        cout << "--- [Finished] Making ixn func ---" << endl;
        cout << endl;
        
        // ***************
        // MARK: - RHS for ixn funcs
        // ***************
        
        cout << "--- Making diff eq rhs ---" << endl;
        
        // Domain1D* dom_wAX1 = new Domain1D(w_ixns.at(0).at("wAX1"), spacing, init_conds.at("wAX1") - 0.5*spacing);
        /*
        Domain1D* dom_wAY1 = new Domain1D(w_ixns.at(0).at("wAY1"), spacing, init_conds.at("wAY1") - 0.5*spacing);
        Domain1D* dom_wAZ1 = new Domain1D(w_ixns.at(0).at("wAZ1"), spacing, init_conds.at("wAZ1") - 0.5*spacing);
        Domain1D* dom_wBX1 = new Domain1D(w_ixns.at(0).at("wBX1"), spacing, init_conds.at("wBX1") - 0.5*spacing);
         */
        // Domain1D* dom_wBY1 = new Domain1D(w_ixns.at(0).at("wBY1"), spacing, init_conds.at("wBY1") - 0.5*spacing);
        /*
        Domain1D* dom_wBZ1 = new Domain1D(w_ixns.at(0).at("wBZ1"), spacing, init_conds.at("wBZ1") - 0.5*spacing);
        Domain1D* dom_wCX1 = new Domain1D(w_ixns.at(0).at("wCX1"), spacing, init_conds.at("wCX1") - 0.5*spacing);
        Domain1D* dom_wCY1 = new Domain1D(w_ixns.at(0).at("wCY1"), spacing, init_conds.at("wCY1") - 0.5*spacing);
         */
        // Domain1D* dom_wCZ1 = new Domain1D(w_ixns.at(0).at("wCZ1"), spacing, init_conds.at("wCZ1") - 0.5*spacing);
        // dom = std::vector<Domain1D*>({dom_wAX1,dom_wBY1,dom_wCZ1});
        
        Domain1D* dom_hA = new Domain1D(bias.at(0).at("hA"), spacing, init_conds.at("hA") - 0.5*spacing);
        Domain1D* dom_hB = new Domain1D(bias.at(0).at("hB"), spacing, init_conds.at("hB") - 0.5*spacing);
        Domain1D* dom_hC = new Domain1D(bias.at(0).at("hC"), spacing, init_conds.at("hC") - 0.5*spacing);
        dom = std::vector<Domain1D*>({dom_hA,dom_hB,dom_hC});
        /*
        Domain1D* dom_bX1 = new Domain1D(bias.at(1).at("bX1"), spacing_b, init_conds.at("bX1") - 0.5*spacing_b);
        Domain1D* dom_bY1 = new Domain1D(bias.at(1).at("bY1"), spacing_b, init_conds.at("bY1") - 0.5*spacing_b);
        Domain1D* dom_bZ1 = new Domain1D(bias.at(1).at("bZ1"), spacing_b, init_conds.at("bZ1") - 0.5*spacing_b);
         */
        /*
        Domain1D* dom_wAX1 = new Domain1D(w_ixns.at(0).at("wAX1"), spacing, init_conds.at("wAX1") - 0.5*spacing);
        Domain1D* dom_wBY1 = new Domain1D(w_ixns.at(0).at("wBY1"), spacing, init_conds.at("wBY1") - 0.5*spacing);
        Domain1D* dom_wCZ1 = new Domain1D(w_ixns.at(0).at("wCZ1"), spacing, init_conds.at("wCZ1") - 0.5*spacing);
         */

        /*
        domB0 = std::vector<Domain1D*>({dom_hA,dom_hB,dom_hC});
        domB1 = std::vector<Domain1D*>({dom_bX1,dom_bY1,dom_bZ1});
        domW0 = std::vector<Domain1D*>({dom_wAX1,dom_wBY1,dom_wCZ1});
         */
        /*
        domA = std::vector<Domain1D*>({dom_hA,dom_wAX1,dom_bX1});
        domB = std::vector<Domain1D*>({dom_hB,dom_wBY1,dom_bY1});
        domC = std::vector<Domain1D*>({dom_hC,dom_wCZ1,dom_bZ1});
        */
        
        /*
        if (no_hidden_layers > 1) {
            Domain1D* dom_bX2 = new Domain1D(bias.at(2).at("bX2"), spacing, init_conds.at("bX2") - 0.5*spacing);
            Domain1D* dom_bY2 = new Domain1D(bias.at(2).at("bY2"), spacing, init_conds.at("bY2") - 0.5*spacing);
            Domain1D* dom_bZ2 = new Domain1D(bias.at(2).at("bZ2"), spacing, init_conds.at("bZ2") - 0.5*spacing);
            domB2 = std::vector<Domain1D*>({dom_bX2,dom_bY2,dom_bZ2});
        };
        */
         
        std::shared_ptr<DiffEqRHS> rhs;
        for (auto ixn: ixns) {
            /*
            if (ixn->get_type() == IxnParamType::H) {
                rhs = make_shared<DiffEqRHS>("rhs " + ixn->get_name(), ixn, domB0, lrs.at(ixn->get_name()));
            } else if (ixn->get_type() == IxnParamType::B) {
                rhs = make_shared<DiffEqRHS>("rhs " + ixn->get_name(), ixn, domB1, lrs.at(ixn->get_name()));
            } else if (ixn->get_type() == IxnParamType::W) {
                rhs = make_shared<DiffEqRHS>("rhs " + ixn->get_name(), ixn, domW0, lrs.at(ixn->get_name()));
            };
            */
            
            /*
            if (ixn->get_name() == "hA" || ixn->get_name() == "hB" || ixn->get_name() == "hC") {
                rhs = make_shared<DiffEqRHS>("rhs " + ixn->get_name(), ixn, domB0, lrs.at(ixn->get_name()));
            } else if (ixn->get_name() == "wAX1" || ixn->get_name() == "wBY1" || ixn->get_name() == "wCZ1") {
                rhs = make_shared<DiffEqRHS>("rhs " + ixn->get_name(), ixn, domB0, lrs.at(ixn->get_name()));
            } else if (ixn->get_name() == "bX1" || ixn->get_name() == "bY1" || ixn->get_name() == "bZ1") {
                rhs = make_shared<DiffEqRHS>("rhs " + ixn->get_name(), ixn, domB0, lrs.at(ixn->get_name()));
            } else if (ixn->get_name() == "wX1X2" || ixn->get_name() == "wY1Y2" || ixn->get_name() == "wZ1Z2") {
                rhs = make_shared<DiffEqRHS>("rhs " + ixn->get_name(), ixn, domB0, lrs.at(ixn->get_name()));
            } else if (ixn->get_name() == "bX2" || ixn->get_name() == "bY2" || ixn->get_name() == "bZ2") {
                rhs = make_shared<DiffEqRHS>("rhs " + ixn->get_name(), ixn, domB0, lrs.at(ixn->get_name()));
            } else {
                std::cout << "SOMETHING WRONG: " << ixn->get_name() << std::endl;
                exit(EXIT_FAILURE);
            };
             */
            /*
            if (ixn->get_name() == "hA" || ixn->get_name() == "wAX1" || ixn->get_name() == "bX1") {
                rhs = make_shared<DiffEqRHS>("rhs " + ixn->get_name(), ixn, dom, lrs.at(ixn->get_name()));
            } else if (ixn->get_name() == "hB" || ixn->get_name() == "wBY1" || ixn->get_name() == "bY1") {
                rhs = make_shared<DiffEqRHS>("rhs " + ixn->get_name(), ixn, dom, lrs.at(ixn->get_name()));
            } else if (ixn->get_name() == "hC" || ixn->get_name() == "wCZ1" || ixn->get_name() == "bZ1") {
                rhs = make_shared<DiffEqRHS>("rhs " + ixn->get_name(), ixn, dom, lrs.at(ixn->get_name()));
            } else {
                std::cout << "SOMETHING WRONG: " << ixn->get_name() << std::endl;
                exit(EXIT_FAILURE);
            };
            */
            
            rhs = make_shared<DiffEqRHS>("rhs " + ixn->get_name(), ixn, dom, lrs.at(ixn->get_name()));
            ixn->set_diff_eq_rhs(rhs);
        };

        // Mag of updates
        /*
        double max_mag = 0.1;
        for (auto ixn: ixns) {
            ixn->get_diff_eq_rhs()->set_mag_max_update(max_mag);
        };
         */
        
        // Diff eq dependencies
        /*
        for (auto ixn: ixns) {
            if (ixn->get_name() == "hA" || ixn->get_name() == "hB" || ixn->get_name() == "hC") {
                bias.at(0).at("hA")->add_diff_eq_dependency(ixn->get_diff_eq_rhs(), 0);
                bias.at(0).at("hB")->add_diff_eq_dependency(ixn->get_diff_eq_rhs(), 1);
                bias.at(0).at("hC")->add_diff_eq_dependency(ixn->get_diff_eq_rhs(), 2);
            } else if (ixn->get_name() == "bX1" || ixn->get_name() == "bY1" || ixn->get_name() == "bZ1" || ixn->get_name() == "wAX1" || ixn->get_name() == "wBY1" || ixn->get_name() == "wCZ1") {
                bias.at(0).at("hA")->add_diff_eq_dependency(ixn->get_diff_eq_rhs(), 0);
                bias.at(0).at("hB")->add_diff_eq_dependency(ixn->get_diff_eq_rhs(), 1);
                bias.at(0).at("hC")->add_diff_eq_dependency(ixn->get_diff_eq_rhs(), 2);
            } else if (ixn->get_name() == "bX2" || ixn->get_name() == "bY2" || ixn->get_name() == "bZ2" || ixn->get_name() == "wX1X2" || ixn->get_name() == "wY1Y2" || ixn->get_name() == "wZ1Z2") {
                bias.at(0).at("hA")->add_diff_eq_dependency(ixn->get_diff_eq_rhs(), 0);
                bias.at(0).at("hB")->add_diff_eq_dependency(ixn->get_diff_eq_rhs(), 1);
                bias.at(0).at("hC")->add_diff_eq_dependency(ixn->get_diff_eq_rhs(), 2);
            } else {
                std::cout << "SOMETHING WRONG" << std::endl;
                exit(EXIT_FAILURE);
            };
        };
         */
        
        /*
        for (auto ixn: ixns) {
            if (ixn->get_name() == "hA" || ixn->get_name() == "wAX1" || ixn->get_name() == "bX1") {
                bias.at(0).at("hA")->add_diff_eq_dependency(ixn->get_diff_eq_rhs(), 0);
                w_ixns.at(0).at("wAX1")->add_diff_eq_dependency(ixn->get_diff_eq_rhs(), 1);
                bias.at(1).at("bX1")->add_diff_eq_dependency(ixn->get_diff_eq_rhs(), 2);
            } else if (ixn->get_name() == "hB" || ixn->get_name() == "wBY1" || ixn->get_name() == "bY1") {
                bias.at(0).at("hB")->add_diff_eq_dependency(ixn->get_diff_eq_rhs(), 0);
                w_ixns.at(0).at("wBY1")->add_diff_eq_dependency(ixn->get_diff_eq_rhs(), 1);
                bias.at(1).at("bY1")->add_diff_eq_dependency(ixn->get_diff_eq_rhs(), 2);
            } else if (ixn->get_name() == "hC" || ixn->get_name() == "wCZ1" || ixn->get_name() == "bZ1") {
                bias.at(0).at("hC")->add_diff_eq_dependency(ixn->get_diff_eq_rhs(), 0);
                w_ixns.at(0).at("wCZ1")->add_diff_eq_dependency(ixn->get_diff_eq_rhs(), 1);
                bias.at(1).at("bZ1")->add_diff_eq_dependency(ixn->get_diff_eq_rhs(), 2);
            } else {
                std::cout << "SOMETHING WRONG" << std::endl;
                exit(EXIT_FAILURE);
            };
        };
         */
        
        /*
        for (auto ixn: ixns) {
            if (ixn->get_type() == IxnParamType::H) {
                bias.at(0).at("hA")->add_diff_eq_dependency(ixn->get_diff_eq_rhs(), 0);
                bias.at(0).at("hB")->add_diff_eq_dependency(ixn->get_diff_eq_rhs(), 1);
                bias.at(0).at("hC")->add_diff_eq_dependency(ixn->get_diff_eq_rhs(), 2);
            } else if (ixn->get_type() == IxnParamType::B) {
                bias.at(1).at("bX1")->add_diff_eq_dependency(ixn->get_diff_eq_rhs(), 0);
                bias.at(1).at("bY1")->add_diff_eq_dependency(ixn->get_diff_eq_rhs(), 1);
                bias.at(1).at("bZ1")->add_diff_eq_dependency(ixn->get_diff_eq_rhs(), 2);
            } else if (ixn->get_type() == IxnParamType::W) {
                w_ixns.at(0).at("wAX1")->add_diff_eq_dependency(ixn->get_diff_eq_rhs(), 0);
                w_ixns.at(0).at("wBY1")->add_diff_eq_dependency(ixn->get_diff_eq_rhs(), 1);
                w_ixns.at(0).at("wCZ1")->add_diff_eq_dependency(ixn->get_diff_eq_rhs(), 2);
            };
        };
         */

        for (auto ixn: ixns) {
            /*
            w_ixns.at(0).at("wAX1")->add_diff_eq_dependency(ixn->get_diff_eq_rhs(), 0);
            w_ixns.at(0).at("wBY1")->add_diff_eq_dependency(ixn->get_diff_eq_rhs(), 1);
            w_ixns.at(0).at("wCZ1")->add_diff_eq_dependency(ixn->get_diff_eq_rhs(), 2);
            */
            bias.at(0).at("hA")->add_diff_eq_dependency(ixn->get_diff_eq_rhs(), 0);
            bias.at(0).at("hB")->add_diff_eq_dependency(ixn->get_diff_eq_rhs(), 1);
            bias.at(0).at("hC")->add_diff_eq_dependency(ixn->get_diff_eq_rhs(), 2);;
        };
        
        cout << "--- [Finished] Making diff eq rhs ---" << endl;
        cout << endl;

        // ***************
        // MARK: - Adjoint
        // ***************
        
        cout << "--- Making adjoint ---" << endl;

        for (auto ixn: ixns) {
            auto adjoint = make_shared<Adjoint>("adjoint "+ixn->get_name(),ixn);
            ixn->set_adjoint(adjoint);
        };
        
        cout << "--- [Finished] Making adjoint ---" << endl;
        cout << endl;

        // ***************
        // MARK: - Lattice
        // ***************
        
        cout << "--- Making lattice ---" << endl;
        
        int side_length = 10;
        latt = make_shared<LatticeTraj>(3, side_length, species_vec[layer_v], mode);
        
        cout << " > begin visible" << endl;
        
        // Add visible bias
        latt->set_bias_of_layer(0, species_map[layer_v]["A"], bias[layer_v]["hA"]);
        latt->set_bias_of_layer(0, species_map[layer_v]["B"], bias[layer_v]["hB"]);
        latt->set_bias_of_layer(0, species_map[layer_v]["C"], bias[layer_v]["hC"]);
        
        cout << " > end visible" << endl;
        
        cout << " > begin hidden" << endl;
        
        for (auto layer_h=1; layer_h<=no_hidden_layers; layer_h++) {

            // Add layer
            latt->add_layer(layer_h, side_length, species_vec.at(layer_h));
            
            // Connectivity
            for (auto i=1; i<=side_length; i++) {
                for (auto j=1; j<=side_length; j++) {
                    for (auto k=1; k<=side_length; k++) {
                        
                        // Displacements
                        for (auto i2=0; i2<=1; i2++) {
                            for (auto j2=0; j2<=1; j2++) {
                                for (auto k2=0; k2<=1; k2++) {
                                    auto i3 = i+i2;
                                    if (i3 > side_length) {
                                        i3 = 1;
                                    };
                                    auto j3 = j+j2;
                                    if (j3 > side_length) {
                                        j3 = 1;
                                    };
                                    auto k3 = k+k2;
                                    if (k3 > side_length) {
                                        k3 = 1;
                                    };
                                    
                                    latt->add_conn(layer_h-1, i, j, k, layer_h, i3, j3, k3);
                                };
                            };
                        };
                    };
                };
            };
            
            // Biases
            latt->set_bias_of_layer(layer_h, species_map.at(layer_h).at("X"+pad_str(layer_h,1)), bias.at(layer_h).at("bX"+pad_str(layer_h,1)));
            latt->set_bias_of_layer(layer_h, species_map.at(layer_h).at("Y"+pad_str(layer_h,1)), bias.at(layer_h).at("bY"+pad_str(layer_h,1)));
            latt->set_bias_of_layer(layer_h, species_map.at(layer_h).at("Z"+pad_str(layer_h,1)), bias.at(layer_h).at("bZ"+pad_str(layer_h,1)));

            // Ixns
            if (layer_h == 1) {
                latt->set_ixn_between_layers(0, species_map[layer_v]["A"], 1, species_map[layer_h]["X"+pad_str(layer_h,1)],w_ixns[layer_v]["wAX"+pad_str(layer_h,1)]);
                //latt->set_ixn_between_layers(0, species_map[layer_v]["A"], 1, species_map[layer_h]["Y"+pad_str(layer_h,1)],w_ixns[layer_v]["wAY"+pad_str(layer_h,1)]);
                //latt->set_ixn_between_layers(0, species_map[layer_v]["A"], 1, species_map[layer_h]["Z"+pad_str(layer_h,1)],w_ixns[layer_v]["wAZ"+pad_str(layer_h,1)]);
                //latt->set_ixn_between_layers(0, species_map[layer_v]["B"], 1, species_map[layer_h]["X"+pad_str(layer_h,1)],w_ixns[layer_v]["wBX"+pad_str(layer_h,1)]);
                latt->set_ixn_between_layers(0, species_map[layer_v]["B"], 1, species_map[layer_h]["Y"+pad_str(layer_h,1)],w_ixns[layer_v]["wBY"+pad_str(layer_h,1)]);
                //latt->set_ixn_between_layers(0, species_map[layer_v]["B"], 1, species_map[layer_h]["Z"+pad_str(layer_h,1)],w_ixns[layer_v]["wBZ"+pad_str(layer_h,1)]);
                //latt->set_ixn_between_layers(0, species_map[layer_v]["C"], 1, species_map[layer_h]["X"+pad_str(layer_h,1)],w_ixns[layer_v]["wCX"+pad_str(layer_h,1)]);
                //latt->set_ixn_between_layers(0, species_map[layer_v]["C"], 1, species_map[layer_h]["Y"+pad_str(layer_h,1)],w_ixns[layer_v]["wCY"+pad_str(layer_h,1)]);
                latt->set_ixn_between_layers(0, species_map[layer_v]["C"], 1, species_map[layer_h]["Z"+pad_str(layer_h,1)],w_ixns[layer_v]["wCZ"+pad_str(layer_h,1)]);

            } else {
                latt->set_ixn_between_layers(layer_h-1, species_map[layer_h-1]["X"+pad_str(layer_h-1,1)], layer_h, species_map[layer_h]["X"+pad_str(layer_h,1)], w_ixns[layer_h-1]["wX"+pad_str(layer_h-1,1)+"X"+pad_str(layer_h,1)]);
                latt->set_ixn_between_layers(layer_h-1, species_map[layer_h-1]["Y"+pad_str(layer_h-1,1)], layer_h, species_map[layer_h]["Y"+pad_str(layer_h,1)], w_ixns[layer_h-1]["wY"+pad_str(layer_h-1,1)+"Y"+pad_str(layer_h,1)]);
                latt->set_ixn_between_layers(layer_h-1, species_map[layer_h-1]["Z"+pad_str(layer_h-1,1)], layer_h, species_map[layer_h]["Z"+pad_str(layer_h,1)], w_ixns[layer_h-1]["wZ"+pad_str(layer_h-1,1)+"Z"+pad_str(layer_h,1)]);
            };
        };
        
        cout << " > end hidden" << endl;
        
        cout << "--- [Finished] Making lattice ---" << endl;
        cout << endl;
        
    };
    
    // ***************
    // MARK: - Destructor
    // ***************
    
    ~DBM() {
        for (auto d: dom) {
            if (d) {
                delete d;
            };
            d = nullptr;
        };
        /*
        for (auto d: domB0) {
            if (d) {
                delete d;
            };
            d = nullptr;
        };
        for (auto d: domB1) {
            if (d) {
                delete d;
            };
            d = nullptr;
        };
        for (auto d: domW0) {
            if (d) {
                delete d;
            };
            d = nullptr;
        };
         */
    };
};


