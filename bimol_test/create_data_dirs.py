import os

parent = "data_learned_hidden"
if not os.path.exists(parent):
	os.makedirs(parent)

dirname = "trajs"
if not os.path.exists(parent+"/"+dirname):
	os.makedirs(parent+"/"+dirname)

dirname = "samples"
if not os.path.exists(parent+"/"+dirname):
	os.makedirs(parent+"/"+dirname)

parent = "data_learned_visible"
if not os.path.exists(parent):
	os.makedirs(parent)
	
dirname = "trajs"
if not os.path.exists(parent+"/"+dirname):
	os.makedirs(parent+"/"+dirname)

dirname = "samples"
if not os.path.exists(parent+"/"+dirname):
	os.makedirs(parent+"/"+dirname)
