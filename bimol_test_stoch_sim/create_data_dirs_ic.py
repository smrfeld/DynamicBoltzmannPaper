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