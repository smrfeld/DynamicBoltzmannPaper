# Test the model for bimolecular annihilation A+A->0

For the reaction `A + A -> 0`.

## Create data directories

Run the included Python script
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

This requires the stochastic simulations in the `bimol_test_stoch_sim` directory, and a trained model in the `bimol_train` directory.

### Trajectories for both models

In `bimol_test_stoch_sim`, new initial conditions are sampled. To evolve these forward in time using the learned differential equation model in `bimol_train`, run the following commands.

From the build directory, for the visible model:
```
make learned_trajs_visible
cd ../bin
./learned_trajs_visible
```
and for the hidden model:
```
make learned_trajs_visible
cd ../bin
./learned_trajs_visible
```
The data is written to `data_learned_hidden/trajs` and `data_learned_visible/trajs`.

The resulting trajectories can be visualized using the `learned_trajs_hidden` and `learned_trajs_visible` notebooks in the `mathematica` directory.

### Sample the learned trajectories

To sample lattices from the learned trajectories, run from the build directory:
```
make sample_hidden
cd ../bin
./sample_hidden
```
or
```
make sample_visible
cd ../bin
./sample_visible
```
The data is written to `data_learned_hidden/samples` and `data_learned_visible/samples`.

### Calculate RMSE

To calculate the RMSE between the stochastic simulations in `bimol_test_stoch_sim` and the moments calculated from the sampled lattices in `data_learned_hidden/samples` and `data_learned_visible/samples`, use the `mse` notebook in the `mathematica` directory.

## Paper figures

Paper figures are created using the `mse` notebook in the `mathematica` directory.
