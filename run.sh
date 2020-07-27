#!/bin/sh

# check if build directory exists
if [ -d "build" ]
then
	# check if the run directory does not exist
	if [ ! -d "run" ]
	then
		# create the run directory
		mkdir run
	fi
	
	# setup the run directory
	./setup-run.sh
	
	# enter the run directory
	cd run
	
	# run the executable
	../build/game
	
	# exit the run directory
	cd ..
else
	# the build directory does not exist
	echo "Build directory does not exist - you must build the project before running"
fi
