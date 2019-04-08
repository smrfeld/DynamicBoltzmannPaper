import os

# Parent
parent = "data"
if not os.path.exists(parent):
	os.makedirs(parent)

# No samples
nSamples = 300

for i in range(1,nSamples+1):
	dir_name = parent + "/lattice_v%03d" % (i)
	if not os.path.exists(dir_name):
		os.makedirs(dir_name)
	dir_name = parent + "/lattice_v%03d/counts" % (i)
	if not os.path.exists(dir_name):
		os.makedirs(dir_name)
	dir_name = parent + "/lattice_v%03d/lattice" % (i)
	if not os.path.exists(dir_name):
		os.makedirs(dir_name)
	dir_name = parent + "/lattice_v%03d/nns" % (i)
	if not os.path.exists(dir_name):
		os.makedirs(dir_name)

# Means
parent = "data_means"
if not os.path.exists(parent):
	os.makedirs(parent)