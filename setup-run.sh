#!/bin/sh

# check if the run directory does not exist
if [ ! -d "run" ]
then
	# create the run directory
	mkdir run
fi
	
# check if a link to the resources directory exists in the run directory
if [ -d "run/res" ] || [ -L "run/res" ]
then
	# remove the resources directory link from the run directory
	rm -r "run/res"
fi
	
# check if the resources directory exists
if [ -d "res" ]
then
	# create a link to the resources directory in the run directory
	if [[ "$OSTYPE" == "cygwin" ]] || [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]
	then
		cmd <<< "mklink /D \"run\res\" \"..\res\"" > /dev/null
	else		
		cwd=$(pwd)
		ln -s "$cwd/res" "$cwd/run/res"
	fi
fi
