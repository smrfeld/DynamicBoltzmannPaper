# Code used in the paper

## Requirements / libraries used

* [Dynamic Boltzmann library](https://github.com/smrfeld/DynamicBoltzmann).
	* This further requires:
	* [Q3C1 library](https://github.com/smrfeld/Q3-C1-Finite-Elements).
	* [Armadillo](http://arma.sourceforge.net).
* [LatticeGillespie library](https://github.com/smrfeld/LatticeGillespieCpp) for stochastic simulations.

## Contents

Folders:
* `hermite`: this generates a simple figure of the Hermite polynomials.
* [bimol_train_stoch_sim](bimol_train_stoch_sim): generates training data for the `A+A->0` reaction.
* [bimol_train](bimol_train): trains the hidden & visible layer models for the `A+A->0` reaction.
* [bimol_test_stoch_sim](bimol_test_stoch_sim): generates testing data for the `A+A->0` reaction.
* [bimol_test](bimol_test): tests the learned differential equations system for visible & hidden layer models for the `A+A->0` reaction.
