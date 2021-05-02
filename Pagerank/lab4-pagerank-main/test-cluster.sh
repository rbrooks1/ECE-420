#! /bin/bash
# Generate matrix data and run main 100 times
# 
# Usage:
#	./test.sh array_size num_threads
# Notes:
#	This shell script should be in the same directory as main and serialtester.
#

if [ $# -lt 1 ]; then
	echo "./test.sh num_processes"
	exit -1
fi

# Parameters
runs=10

num_processes=${1}

clear

echo "Running main"
ATTEMPT=0
while [[ $ATTEMPT -ne $runs ]]; do
	let ATTEMPT+=1
	echo "Attempt ${ATTEMPT} started"
	mpirun -np $num_processes -f ./hosts ./main && ./serialtester
	sed '2q;d' ./data_output >> ./pagerank_times
done
echo "done"
