#!/bin/sh
# This script ccepts two runtime arguments
# filesdir: a path to a directory on the filesystem
# searchstr: a text string which will be searched within these files

# Incorrect number of input arguments
if [ $# -ne 2 ]
then
	echo "Input arguments not equal to 2!"
	exit 1
fi

# The first argument is not a directory
if [ ! -d $1 ]
then
	echo "The first argument should be a directory or the directory does not exist!"
	exit 1
fi

# Count the total number of files in the directory
# and the number of matching lines
X=$(find $1 -type f|wc -l)
Y=$(grep -r $1 -e $2 |wc -l)
echo "The number of files are $X and the number of matching lines are $Y"


