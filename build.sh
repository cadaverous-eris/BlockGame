#!/bin/sh

# check if build directory does not exist
if [ ! -d "build" ]
then
	# create build directory
	mkdir build
fi

# enter the build directory
cd build

# run the generated makefile
cmake --build .

# exit the build directory
cd ..
