To run:

./em_algorithm_parallel_ubuntu.sh <input_folder>


The output ranks are stored in the `ranks` folder.

make NUM_THREAD=4 WORKING_CHUNK_SIZE=8388608

for big input:
make NUM_THREAD=24 WORKING_CHUNK_SIZE=33554432
