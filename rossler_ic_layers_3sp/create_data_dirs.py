import os

# Parent
parent = "data"
if not os.path.exists(parent):
	os.makedirs(parent)

# sub
dir_name = parent + "/sample_1layers"
if not os.path.exists(dir_name):
	os.makedirs(dir_name)

tStart = 0
tEnd = 190 # inclusive
tSkip = 10

for i in range(tStart,tEnd+tSkip,tSkip):

	dir_name = parent + "/sample_1layers/%04d" % i
	if not os.path.exists(dir_name):
		os.makedirs(dir_name)

# sub
dir_name = parent + "/fine_tune_1layers"
if not os.path.exists(dir_name):
	os.makedirs(dir_name)

tStart = 0
tEnd = 190 # inclusive
tSkip = 10

for i in range(tStart,tEnd+tSkip,tSkip):

	dir_name = parent + "/fine_tune_1layers/%04d" % i
	if not os.path.exists(dir_name):
		os.makedirs(dir_name)