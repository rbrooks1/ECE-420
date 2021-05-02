#! /bin/bash
# Generate matrix data and run main 100 times
# 
# Usage:
#	./test.sh array_size num_threads
# Notes:
#	This shell script should be in the same directory as main and serialtester.
#

if [ $# -lt 2 ]; then
	echo "./test.sh array_size num_threads"
	exit -1
fi

# Parameters
runs=100

array_size=${1}
num_threads=${2}

clear

echo "Running main"
ATTEMPT=0
while [[ $ATTEMPT -ne $runs ]]; do
	let ATTEMPT+=1
	echo "Attempt ${ATTEMPT} started"
	./main $num_threads && ./serialtester
	tail ./data_output -n 1 >> ./solver_times
	echo "" >> ./solver_times
done
echo "done"
