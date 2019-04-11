#include <iostream>
#include <math.h>
#include <vector>
#include <iomanip>
#include <sstream>

// Get the header
#include <lattgillespie>

using namespace lattg;

// Function to get the mean of a vector
double get_mean(std::vector<int> v) {
	double ctr = 0.0;
	for (auto i: v) { ctr += i; };
	return ctr / v.size();
};

// Function to get the std dev of a vector
double get_std(std::vector<int> v) {
	double mean = get_mean(v);
	double ctr = 0.0;
	for (auto i: v) { ctr += pow(i - mean,2); };
	return sqrt(ctr / (v.size()-1));
};

/********************
Zero pad a string
********************/

std::string pad_str(int i, int n_zeros) {
	std::stringstream fname;
	fname << std::setfill('0') << std::setw(n_zeros) << i;
	return fname.str();
};

int main() {

	/********************
	Params
	********************/

	// Seed random no
	srand (time(NULL));
	// srand (2);

	// Rates
 	double kp1=30.0, km1=0.25, kp2=1.0, kp3=10.0, kp4=0.6, kp5=16.5, km5=0.9;

	// Box length
	int box_length = 10;

	// Timestep
	double dt = 0.01;

	// Number of steps to run
	// int n_steps = 100;
	int n_steps = 250;

	// Reaction probs
	double m = 0.4;
	double probm1 = m*km1;
	double probp2 = m*kp2;
	double probp4 = m*kp4;
	double probm5 = m*km5;

	// Run
	bool verbose = true;
	bool write_counts = true;
	bool write_nns = true;
	bool write_latt = true;
	int write_step = 1;

	// 200 particles ~ log (0.2/(1-3*0.2)) = - log(2)
	std::map<std::string,int> pops;
	pops["A"] = 200;
	pops["B"] = 200;
	pops["C"] = 200;
	/*
	std::map<std::string,double> hdict;
	hdict["A"] = -0.69;
	hdict["B"] = -0.69;
	hdict["C"] = -0.69;
	std::map<std::string,std::map<std::string,double>> jdict;
	jdict["A"]["A"] = 0;
	jdict["A"]["B"] = 0;
	jdict["A"]["C"] = 0;
	jdict["B"]["B"] = 0;
	jdict["B"]["C"] = 0;
	jdict["C"]["C"] = 0;
	*/

	// Number of samples to run
	int i_start = 60;
	int i_end = 100; // inclusive
	std::vector<int> versions({95});

	for (auto write_version_no: versions) {
	// for (int write_version_no=i_start; write_version_no<=i_end; write_version_no++) {

		std::cout << "------ " << write_version_no << " -------" << std::endl;

		/****************************************
		Make a simulation!
		****************************************/

		// Make a simulation
		Simulation sim(dt,box_length);

		/********************
		Add species
		********************/

		sim.add_species("A");
		sim.add_species("B");
		sim.add_species("C");

		/********************
		Add unimol rxns
		********************/

		// P1
		sim.add_uni_rxn("rxnP1", kp1, "A","A","A");
		// P3
		sim.add_uni_rxn("rxnP3", kp3, "B");
		// P5
		sim.add_uni_rxn("rxnP5", kp5, "C","C","C");

		/********************
		Add bimol rxns
		********************/

		// M1
		sim.add_bi_rxn("rxnM1", probm1, "A","A","A");
		// P2
		sim.add_bi_rxn("rxnP2", probp2, "A","B","B","B");
		// P4
		sim.add_bi_rxn("rxnP4", probp4, "A","C");
		// M5
		sim.add_bi_rxn("rxnM5", probm5, "C","C","C");

		/********************
		Populate the lattice
		********************/

		sim.populate_lattice(pops);
		// sim.populate_lattice(hdict,jdict,10);
		// sim.read_lattice("../data/lattice_v"+pad_str(write_version_no,3)+"/lattice/0000.txt");

		/********************
		Run
		********************/

		sim.run(n_steps,verbose,write_counts,write_nns,write_latt,write_step,write_version_no,"../data");
	};

	/********************
	Write
	********************/

	// Write the lattice to a file
	// sim.write_lattice(0);

	// Write the lattice to a file
	// sim.write_lattice(n_steps);

	return 0;
}
