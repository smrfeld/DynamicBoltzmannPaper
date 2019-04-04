# Generate training data for bimolecular annihilation A+A->0

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

### Stochastic simulations

From the build directory:
```
make main
cd ../bin
./main
```

They can be visualized with the `visualize_lattice` notebook in the `mathematica` directory.

The first moments in the data can be visualized using the `moments` notebook.

### Learning initial conditions

From the build directory:
```
make learn_ic
cd ../bin
./learn_ic
```

The progress can be monitored with the `monitor_progress_learn_ic` notebook in the `mathematica` directory.

The final learned points can be visualized using the `ic_learned` notebook.

## Paper figures

Can be generated using the `figures` notebook in the `mathematica` directory. In particular:
* `visualize_lattice` generates two example lattices.
* `moments` visualizes the first moment of the data.