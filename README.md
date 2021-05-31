# Code used in the paper

* [Phys. Rev. E 99, 063315](https://journals.aps.org/pre/abstract/10.1103/PhysRevE.99.063315)
* [arXiv:1808.08630](https://arxiv.org/abs/1808.08630)

## Requirements / libraries used

* [Dynamic Boltzmann library](https://github.com/smrfeld/DynamicBoltzmann), specifically v3.0 [here](https://github.com/smrfeld/DynamicBoltzmann/releases/tag/3.0).
	* This further requires:
	* [Q3C1 library](https://github.com/smrfeld/Q3-C1-Finite-Elements), specifically v2.0 [here](https://github.com/smrfeld/Q3-C1-Finite-Elements/releases/tag/2.0).
	* [Armadillo](http://arma.sourceforge.net), specifically v9.300.2 [here](http://arma.sourceforge.net/download.html).
* [LatticeGillespie library](https://github.com/smrfeld/LatticeGillespieCpp) for stochastic simulations, specifically v1.0 [here](https://github.com/smrfeld/LatticeGillespieCpp/releases/tag/1.0).

## Contents

Folders:
* [hermite](hermite): this generates a simple figure of the Hermite polynomials.
* [bimol_train_stoch_sim](bimol_train_stoch_sim): generates training data for the `A+A->0` reaction.
* [bimol_train](bimol_train): trains the hidden & visible layer models for the `A+A->0` reaction.
* [bimol_test_stoch_sim](bimol_test_stoch_sim): generates testing data for the `A+A->0` reaction.
* [bimol_test](bimol_test): tests the learned differential equations system for visible & hidden layer models for the `A+A->0` reaction.
* [rossler_stoch_sims](rossler_stoch_sims): generates training data for the Rossler system.
* [rossler_dynamic_3sp](rossler_dynamic_3sp): learns and tests the Rossler system.
