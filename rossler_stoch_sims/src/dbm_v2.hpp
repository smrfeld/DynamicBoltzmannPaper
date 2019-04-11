#include <iostream>
#include <bmla>
#include <fstream>
#include <vector>
#include <map>

#include <sstream>

using namespace dblz;
using namespace std;

struct DBM {
    
    // ***************
    // MARK: - Members
    // ***************
    
    // No hidden layers
    int no_hidden_layers;
    
    // Species
    map<int,map<string,shared_ptr<Species>>> species_map;
    map<int,vector<shared_ptr<Species>>> species_vec;
    
    // Ixn funcs
    map<int,map<string,Iptr>> bias;
    map<int,map<string,Iptr>> w_ixns;
    vector<Iptr> ixns;
    
    // Lattice
    shared_ptr<Lattice> latt;
    
    // ***************
    // MARK: - Constructor
    // ***************

    DBM(int no_hidden_layers) {
        
        this->no_hidden_layers = no_hidden_layers;
        
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
        w_ixns[layer_v][ixn_name] = make_shared<IxnParam>(ixn_name,IxnParamType::W, 0.0);
        ixns.push_back(w_ixns[layer_v][ixn_name]);
        
        ixn_name = "wBX1";
        w_ixns[layer_v][ixn_name] = make_shared<IxnParam>(ixn_name,IxnParamType::W, 0.0);
        ixns.push_back(w_ixns[layer_v][ixn_name]);
        
        ixn_name = "wCX1";
        w_ixns[layer_v][ixn_name] = make_shared<IxnParam>(ixn_name,IxnParamType::W, 0.0);
        ixns.push_back(w_ixns[layer_v][ixn_name]);
            
        // Bias visibles
        ixn_name = "hA";
        bias[layer_v][ixn_name] = make_shared<IxnParam>(ixn_name,IxnParamType::H, 0.0);
        ixns.push_back(bias[layer_v][ixn_name]);
        
        ixn_name = "hB";
        bias[layer_v][ixn_name] = make_shared<IxnParam>(ixn_name,IxnParamType::H, 0.0);
        ixns.push_back(bias[layer_v][ixn_name]);
        
        ixn_name = "hC";
        bias[layer_v][ixn_name] = make_shared<IxnParam>(ixn_name,IxnParamType::H, 0.0);
        ixns.push_back(bias[layer_v][ixn_name]);

        // Hidden layers
        for (auto layer_h=1; layer_h<=no_hidden_layers; layer_h++) {

            // Ws
            if (layer_h != no_hidden_layers) {
                ixn_name = "wX" + pad_str(layer_h,1) + "X" + pad_str(layer_h+1,1);
                w_ixns[layer_h][ixn_name] = make_shared<IxnParam>(ixn_name,IxnParamType::W, 0.0);
                ixns.push_back(w_ixns[layer_h][ixn_name]);
            };
            
            // Bias hidden
            ixn_name = "bX" + pad_str(layer_h,1);
            bias[layer_h][ixn_name] = make_shared<IxnParam>(ixn_name,IxnParamType::B, 0.0);
            ixns.push_back(bias[layer_h][ixn_name]);
        };
        
        cout << "--- [Finished] Making ixn func ---" << endl;
        cout << endl;

        // ***************
        // MARK: - Lattice
        // ***************
        
        cout << "--- Making lattice ---" << endl;
        
        int side_length = 10;
        latt = make_shared<Lattice>(3,side_length,species_vec[layer_v],false);
        
        cout << " > begin visible" << endl;
        
        // Add visible bias
        latt->add_bias_to_layer(0, species_map[layer_v]["A"], bias[layer_v]["hA"]);
        latt->add_bias_to_layer(0, species_map[layer_v]["B"], bias[layer_v]["hB"]);
        latt->add_bias_to_layer(0, species_map[layer_v]["C"], bias[layer_v]["hC"]);
        
        cout << " > end visible" << endl;
        
        cout << " > begin hidden" << endl;

        int side_length_hidden = side_length;
        for (auto layer_h=1; layer_h<=no_hidden_layers; layer_h++) {

            side_length_hidden -= 2;
            
            // Add layer
            latt->add_layer(layer_h, side_length_hidden, species_vec[layer_h]);
            
            // Hidden layer units
            for (auto i=1; i<=side_length_hidden; i++) {
                for (auto j=1; j<=side_length_hidden; j++) {
                    for (auto k=1; k<=side_length_hidden; k++) {
                        
                        // Visible unit to connect to
                        // Displacements
                        for (auto i2=0; i2<=2; i2++) {
                            for (auto j2=0; j2<=2; j2++) {
                                for (auto k2=0; k2<=2; k2++) {
                                    auto i3 = i+i2;
                                    auto j3 = j+j2;
                                    auto k3 = k+k2;
                                    
                                    latt->add_conn(layer_h-1, i3, j3, k3, layer_h, i, j, k);
                                };
                            };
                        };
                    };
                };
            };
            
            // Biases
            latt->add_bias_to_layer(layer_h, species_map[layer_h]["X"+pad_str(layer_h,1)], bias[layer_h]["bX"+pad_str(layer_h,1)]);
            
            // Ixns
            if (layer_h == 1) {
                latt->add_ixn_between_layers(0, species_map[layer_v]["A"], 1, species_map[layer_h]["X"+pad_str(layer_h,1)],w_ixns[layer_v]["wAX"+pad_str(layer_h,1)]);
                latt->add_ixn_between_layers(0, species_map[layer_v]["B"], 1, species_map[layer_h]["X"+pad_str(layer_h,1)],w_ixns[layer_v]["wBX"+pad_str(layer_h,1)]);
                latt->add_ixn_between_layers(0, species_map[layer_v]["C"], 1, species_map[layer_h]["X"+pad_str(layer_h,1)],w_ixns[layer_v]["wCX"+pad_str(layer_h,1)]);
                
            } else {
                latt->add_ixn_between_layers(layer_h-1, species_map[layer_h-1]["X"+pad_str(layer_h-1,1)], layer_h, species_map[layer_h]["X"+pad_str(layer_h,1)], w_ixns[layer_h-1]["wX"+pad_str(layer_h-1,1)+"X"+pad_str(layer_h,1)]);
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
    };
};

