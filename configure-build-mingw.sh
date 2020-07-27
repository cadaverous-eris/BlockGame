#!/bin/sh

# check if build directory does not exist
if [ ! -d "build" ]
then
	# create build directory
	mkdir build
fi

# enter the build directory
cd build

# generate makefile with cmake
cmake .. -G "MinGW Makefiles"

# exit the build directory
cd ..
