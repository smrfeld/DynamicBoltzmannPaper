import os

# Parent
parent = "data_learn_ic"
if not os.path.exists(parent):
	os.makedirs(parent)

# Subdirs
dir_name = parent + "/moments"
if not os.path.exists(dir_name):
	os.makedirs(dir_name)

dir_name = parent + "/ixn_params"
if not os.path.exists(dir_name):
	os.makedirs(dir_name)

# Parent
parent = "data"
if not os.path.exists(parent):
	os.makedirs(parent)

# No IC
nIC = 50;

# No samples for each
nSamples = 50

for j in range(1,nIC+1):
	for i in range(1,nSamples+1):
		dir_name = parent + "/ic_v%03d/lattice_v%03d" % (j,i)
		if not os.path.exists(dir_name):
			os.makedirs(dir_name)
		dir_name = parent + "/ic_v%03d/lattice_v%03d/counts" % (j,i)
		if not os.path.exists(dir_name):
			os.makedirs(dir_name)
		dir_name = parent + "/ic_v%03d/lattice_v%03d/lattice" % (j,i)
		if not os.path.exists(dir_name):
			os.makedirs(dir_name)
		dir_name = parent + "/ic_v%03d/lattice_v%03d/nns" % (j,i)
		if not os.path.exists(dir_name):
			os.makedirs(dir_name)