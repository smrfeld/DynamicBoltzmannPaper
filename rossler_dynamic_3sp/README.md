# Train the model for Rossler oscillator

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

This requires the stochastic simulations in the `rossler_stoch_sims` directory.

### Training

From the build directory:
```
make learn_sequential
cd ../bin
./learn_sequential
```
The data is in the `data/learn_sequential` directory.

The progress can be monitored with the `monitor_progress_ixn_params` and `monitor_progress_moments` notebooks in the `mathematica` directory.

### Sampling the learned trajectories

The learned trajectories can be sampled by running from the build directory:
```
make sample
cd ../bin
./sample
```
The data is in the `data/sample` directory.

### Activating hidden layer

To activate the hidden layer of some lattice configuration, run from the build directory:
```
make activate
cd ../bin
./activate
```
The data is in the `data/activate` directory.

## Paper figures

Paper figures can be generated using the notebooks in the `mathematica` directory:
* `diff_eq_rhs` visualizes the learned right hand sides of the differential equations.
* `sample` visualizes some sampled learned lattices and the hidden layers.
* `moments` compares the learned moments calculated from the sampled lattices with the moments in the stoch sims in `rossler_stoch_sims`.
