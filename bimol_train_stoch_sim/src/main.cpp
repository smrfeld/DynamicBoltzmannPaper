#include <iostream>
#include <math.h>
#include <random>
#include <fstream>
#include <sstream>
#include <iomanip>

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

std::string pad_str(int i, int n_zeros) {
	std::stringstream fname;
	fname << std::setfill('0') << std::setw(n_zeros) << i;
	return fname.str();
};

double randD(double dMin, double dMax)
{
    return dMin + ((double)rand() / RAND_MAX) * (dMax - dMin);
};

int main() {

	/********************
	Params
	********************/

	// Seed random no
	srand (time(NULL));
	// srand (2);

	// Box length
	int box_length = 1000;

	// Timestep
	double dt = 0.01;

	// Number of steps to run
	int n_steps = 101;

	// no sampling steps for CD
	int n_sampling_cd = 1000;

	/********************
	IC
	********************/


	// Initial points in h,J,K space
	std::vector<double> hpts,jpts,kpts;

	// Box of hpts, jpts, kpts
	double hmin=-1.0, hmax=1.0;
	double jmin = -1.0, jmax=1.0;
	double kmin = -1.0, kmax=1.0;

	// No IC
	int nIC = 100;
	for (int i=0; i<nIC; i++ ) {
		hpts.push_back(randD(hmin,hmax));
		jpts.push_back(randD(jmin,jmax));
		kpts.push_back(randD(kmin,kmax));
	};

	// Write the IC
	for (int i=0; i<nIC; i++) {
		std::ofstream f;
		std::string fname = "ic_v"+pad_str(i+1,3)+"/init.txt";
		f.open(fname);
		f << "h " << hpts[i] << "\n";
		f << "J " << jpts[i] << "\n";
		f << "K " << kpts[i];
		f.close();	
	};

	/********************
	Run
	********************/

	// n samples per IC
	int n_samples_per_IC = 100;

	// Go through all
	for (int i=0; i<nIC; i++)
	{
		std::cout << "--- IC: " << i << " ---" << std::endl;

		// Dict versions of IC
		std::map<std::string,double> h_dict;
		std::map<std::string,std::map<std::string,double>> j_dict;		
		std::map<std::string,std::map<std::string,std::map<std::string,double>>> k_dict;		
		h_dict["A"] = hpts[i];
		j_dict["A"]["A"] = jpts[i];
		k_dict["A"]["A"]["A"] = kpts[i];

		for (int j=0; j<n_samples_per_IC; j++)
		{
			std::cout << "--- Simulating version: " << j << " ---" << std::endl;

			// Make a simulation
			Simulation sim(dt,box_length,1); // 1 = 1D

			// Add a species
			sim.add_species("A");

			// Rxn
			sim.add_bi_rxn("ann", 0.01, "A","A");

			// Populate
			sim.populate_lattice(h_dict, j_dict, k_dict, n_sampling_cd);

			// Run
			bool verbose = false;
			bool write_counts = true;
			bool write_nns = true;
			bool write_latt = true;
			int write_step = 1;
			sim.run(n_steps,verbose,write_counts,write_nns,write_latt,write_step,j+1,"../data/ic_v"+pad_str(i+1,3));
		};
	};

	return 0;
}
