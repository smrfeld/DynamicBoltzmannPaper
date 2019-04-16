import os

# Parent
parent = "data"
if not os.path.exists(parent):
	os.makedirs(parent)

# sub
dir_name = parent + "/learn_sequential"
if not os.path.exists(dir_name):
	os.makedirs(dir_name)

dir_name = parent + "/learn_sequential/moments"
if not os.path.exists(dir_name):
	os.makedirs(dir_name)

dir_name = parent + "/learn_sequential/ixn_params"
if not os.path.exists(dir_name):
	os.makedirs(dir_name)

dir_name = parent + "/learn_sequential/diff_eq_rhs"
if not os.path.exists(dir_name):
	os.makedirs(dir_name)

dir_name = parent + "/sample"
if not os.path.exists(dir_name):
	os.makedirs(dir_name)

no_dirs = 100
for i in range(0,no_dirs+1):

	dir_name = parent + "/sample/%04d" % i
	if not os.path.exists(dir_name):
		os.makedirs(dir_name)

dir_name = parent + "/activate"
if not os.path.exists(dir_name):
	os.makedirs(dir_name)