#!/bin/sh
# This script accepts the two arguments
# writefile: a full path to a file (including filename) on the filesystem
# writestr: a text string which will be written within this file

# Incorrect number of input arguments
if [ $# -ne 2 ]
then
	echo "Input arguments not equal to 2!"
	exit 1
fi

# Create the path if not exists
dirc=$(dirname $1)
if [ ! -d $dirc ]
then
	mkdir -p $dirc
fi

touch $1

# If file can be created and we have write permission
if [ -w $1 ]
then
	echo $2 > $1
else
	echo "Cannot create the file or white to the file!"
	exit 1
fi
