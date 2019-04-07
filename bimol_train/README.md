# Train the model for bimolecular annihilation A+A->0

For the reaction `A + A -> 0`.

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

### Input data

This requires the stochastic simulations in the `bimol_train_stoch_sim` directory.

### Learning the hidden layer model

From the build directory:
```
make main_hidden
cd ../bin
./main_hidden
```
The data is in the `data_learned_hidden` directory.

The progress can be monitored with the `hidden_monitor_progress` notebook in the `mathematica` directory.

After learning, the complete trajectories can be constructed by running from the build directory:
```
make learned_trajs_hidden
cd ../bin
./learned_trajs_hidden
```
The data is in the `data_learned_hidden/trajs` directory.

### Learning the visible layer model

As above, replacing `hidden` with `visible`.

## Paper figures

Paper figures can be generated using the notebooks in the `mathematica` directory:
* `hidden_diff_eq_rhs`,`visible_diff_eq_rhs` visualize the learned right hand sides of the differential equations.
* `correlation` calculates the correlations in the stoch sims in `bimol_train_stoch_sim`
* `colored_trajs` creates the nearest-neighbor colored initial lattice configurations.