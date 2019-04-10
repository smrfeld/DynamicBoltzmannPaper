#include <iostream>
#include <bmla>
#include <fstream>
#include <vector>
#include <map>

#include <sstream>

using namespace dblz;
using namespace std;

// ***************
// MARK: - DBM
// ***************

struct DBM {
    
    /********************
     Members
     ********************/
    
    // Mode
    LatticeMode mode;
    
    // No hidden layers
    int no_hidden_layers;
    
    // Species
    map<int,map<string,shared_ptr<Species>>> species_map;
    map<int,vector<shared_ptr<Species>>> species_vec;
    
    // Ixn funcs
    map<int,map<string,shared_ptr<IxnParam>>> bias;
    map<int,map<string,shared_ptr<IxnParam>>> w_ixns;
    vector<shared_ptr<IxnParam>> ixns;
    
    // Scale and shift parameters
    map<int,Iptr> beta;
    map<int,Iptr> gamma;
    
    // Lattice
    shared_ptr<Lattice> latt;
    
    /********************
     Constructor
     ********************/
    
    DBM(int no_hidden_layers, LatticeMode mode) {
        
        this->mode = mode;
        this->no_hidden_layers = no_hidden_layers;
        
        // Make species
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
        
        // Make ixn funcs
        cout << "--- Making ixn func ---" << endl;
        
        std::string ixn_name;
        
        // Layer 0
        // Ws
        ixn_name = "wAX" + pad_str(1,1);
        w_ixns[layer_v][ixn_name] = make_shared<IxnParam>(ixn_name,IxnParamType::W, 0.0, 0.0);
        ixns.push_back(w_ixns[layer_v][ixn_name]);
        
        ixn_name = "wBY" + pad_str(1,1);
        w_ixns[layer_v][ixn_name] = make_shared<IxnParam>(ixn_name,IxnParamType::W, 0.0, 0.0);
        ixns.push_back(w_ixns[layer_v][ixn_name]);
        
        ixn_name = "wCZ" + pad_str(1,1);
        w_ixns[layer_v][ixn_name] = make_shared<IxnParam>(ixn_name,IxnParamType::W, 0.0, 0.0);
        ixns.push_back(w_ixns[layer_v][ixn_name]);
            
        // Bias visibles
        ixn_name = "hA";
        bias[layer_v][ixn_name] = make_shared<IxnParam>(ixn_name,IxnParamType::H, 0.0, 0.0);
        ixns.push_back(bias[layer_v][ixn_name]);
        
        ixn_name = "hB";
        bias[layer_v][ixn_name] = make_shared<IxnParam>(ixn_name,IxnParamType::H, 0.0, 0.0);
        ixns.push_back(bias[layer_v][ixn_name]);
        
        ixn_name = "hC";
        bias[layer_v][ixn_name] = make_shared<IxnParam>(ixn_name,IxnParamType::H, 0.0, 0.0);
        ixns.push_back(bias[layer_v][ixn_name]);

        // Hidden layers
        for (auto layer_h=1; layer_h<=no_hidden_layers; layer_h++) {

            // Ws
            if (layer_h != no_hidden_layers) {
                ixn_name = "wX" + pad_str(layer_h,1) + "X" + pad_str(layer_h+1,1);
                w_ixns[layer_h][ixn_name] = make_shared<IxnParam>(ixn_name,IxnParamType::W, 0.0, 0.0);
                ixns.push_back(w_ixns[layer_h][ixn_name]);

                ixn_name = "wY" + pad_str(layer_h,1) + "Y" + pad_str(layer_h+1,1);
                w_ixns[layer_h][ixn_name] = make_shared<IxnParam>(ixn_name,IxnParamType::W, 0.0, 0.0);
                ixns.push_back(w_ixns[layer_h][ixn_name]);

                ixn_name = "wZ" + pad_str(layer_h,1) + "Z" + pad_str(layer_h+1,1);
                w_ixns[layer_h][ixn_name] = make_shared<IxnParam>(ixn_name,IxnParamType::W, 0.0, 0.0);
                ixns.push_back(w_ixns[layer_h][ixn_name]);
            };
            
            // Bias hidden
            ixn_name = "bX" + pad_str(layer_h,1);
            bias[layer_h][ixn_name] = make_shared<IxnParam>(ixn_name,IxnParamType::B, 0.0, 0.0);
            ixns.push_back(bias[layer_h][ixn_name]);

            ixn_name = "bY" + pad_str(layer_h,1);
            bias[layer_h][ixn_name] = make_shared<IxnParam>(ixn_name,IxnParamType::B, 0.0, 0.0);
            ixns.push_back(bias[layer_h][ixn_name]);

            ixn_name = "bZ" + pad_str(layer_h,1);
            bias[layer_h][ixn_name] = make_shared<IxnParam>(ixn_name,IxnParamType::B, 0.0, 0.0);
            ixns.push_back(bias[layer_h][ixn_name]);

        };
        
        cout << "--- [Finished] Making ixn func ---" << endl;
        cout << endl;
        
        // Lattice
        
        cout << "--- Making lattice ---" << endl;
        
        int side_length = 10;
        latt = make_shared<Lattice>(3,side_length,species_vec[layer_v],mode);
        
        cout << " > begin visible" << endl;
        
        // Add visible bias
        latt->set_bias_of_layer(0, species_map[layer_v]["A"], bias[layer_v]["hA"]);
        latt->set_bias_of_layer(0, species_map[layer_v]["B"], bias[layer_v]["hB"]);
        latt->set_bias_of_layer(0, species_map[layer_v]["C"], bias[layer_v]["hC"]);
        
        cout << " > end visible" << endl;
        
        cout << " > begin hidden" << endl;
        
        for (auto layer_h=1; layer_h<=no_hidden_layers; layer_h++) {

            // Add layer
            latt->add_layer(layer_h, side_length, species_vec[layer_h]);
            
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
                                        i3 = i3-side_length;
                                    };
                                    if (i3 < 1) {
                                        i3 = side_length + i3;
                                    };
                                    
                                    auto j3 = j+j2;
                                    if (j3 > side_length) {
                                        j3 = j3-side_length;
                                    };
                                    if (j3 < 1) {
                                        j3 = side_length + j3;
                                    };

                                    auto k3 = k+k2;
                                    if (k3 > side_length) {
                                        k3 = k3-side_length;
                                    };
                                    if (k3 < 1) {
                                        k3 = side_length + k3;
                                    };

                                    latt->add_conn(layer_h-1, i, j, k, layer_h, i3, j3, k3);
                                };
                            };
                        };
                    };
                };
            };
            
            // Biases
            latt->set_bias_of_layer(layer_h, species_map[layer_h]["X"+pad_str(layer_h,1)], bias[layer_h]["bX"+pad_str(layer_h,1)]);
            latt->set_bias_of_layer(layer_h, species_map[layer_h]["Y"+pad_str(layer_h,1)], bias[layer_h]["bY"+pad_str(layer_h,1)]);
            latt->set_bias_of_layer(layer_h, species_map[layer_h]["Z"+pad_str(layer_h,1)], bias[layer_h]["bZ"+pad_str(layer_h,1)]);

            // Ixns
            if (layer_h == 1) {
                latt->set_ixn_between_layers(0, species_map[layer_v]["A"], 1, species_map[layer_h]["X"+pad_str(layer_h,1)],w_ixns[layer_v]["wAX"+pad_str(layer_h,1)]);
                latt->set_ixn_between_layers(0, species_map[layer_v]["B"], 1, species_map[layer_h]["Y"+pad_str(layer_h,1)],w_ixns[layer_v]["wBY"+pad_str(layer_h,1)]);
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
    
    /********************
     Read params
     ********************/
    
    // Read init conds
    void read_init_conds_increment(string fname) {
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
                nu=""; val="";
            };
        };
        f.close();
        
        for (auto pr: ret) {
            for (auto ixn: ixns) {
                if (ixn->get_name() == pr.first) {
                    ixn->set_val(ixn->get_val()+pr.second);
                    std::cout << "Incrementing: " << ixn->get_name() << " by: " << pr.second << " now: " << ixn->get_val() << std::endl;
                    break;
                };
            };
        };
    };
};


// ***************
// MARK: - Write function
// ***************

void write(std::map<string,double> aves, string fname) {
    std::ofstream f;
    
    // Open
    f.open(fname);
    
    // Make sure we found it
    if (!f.is_open()) {
        std::cerr << ">>> Error: write <<< could not write to file: " << fname << std::endl;
        exit(EXIT_FAILURE);
    };
    
    // Go through all
    int i=0;
    for (auto pr: aves) {
        f << pr.first << " " << pr.second;
        if (i != aves.size()-1) {
            f << "\n";
        };
        i++;
    };
    
    // Close
    f.close();
};
