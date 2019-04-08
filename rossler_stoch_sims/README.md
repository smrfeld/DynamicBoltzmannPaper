# Rossler oscillator stochastic simulations

## Create data directories

Run the included Python scripts
```
python create_data_dirs.py
```

## Running

First use `cmake`:
```
mkdir build
cd build
cmake ..
```

### Stochastic simulations

From the build directory:
```
make main
cd ../bin
./main
```
Data is written to the `data` directory.

The initial configuration is to have 200 particles of each species randomly distributed.

### Sample lattices as initial condition

To instead sample an initial lattice of a chosen configuration, run from the build directory:
```
make sample_ic
cd ../bin
./sample_ic
```
Then run the `main` script with `sim.read_lattice("/path/to/initial/lattice.txt");`.

### Calculate moments

To calculate the means of the lattices over time, run the `calculate_means` notebook in the `mathematica` directory.

To show the moments over time, run the `moments` notebook in the `mathematica` directory.

### Visualizing lattices

To visualize the lattices, use the `visualize_lattice` notebook in the `mathematica` directory.




