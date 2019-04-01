# Generate testing data for bimolecular annihilation A+A->0

For the reaction `A + A -> 0`.

## Create data directories

Run the included Python scripts
```
python create_data_dirs_stoch_sim.py
python create_data_dirs_ic.py
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

They can be visualized with the `visualize_lattice` notebook in the `mathematica` directory.

### Learning initial conditions

From the build directory:
```
make learn_ic
cd ../bin
./learn_ic
```

The progress can be monitored with the `learn_ic` notebook in the `mathematica` directory.

### Calculating higher order moments in the stochastic simulations

Use the `count_triplets_quartics` notebook in the `mathematica` directory.

This writes back to the `data` directory with the stochastic simulations, e.g. it creates under `ic_v001/lattice_v001/` new `triplets` and `quartics` directories for three-in-a-row and four-in-a-row moments.