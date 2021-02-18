**Note:** This version is out of date, and likely contains bugs

This is a preliminary multithreaded version of SuffixRank, using OpenMP.

To run:

```
make NUM_THREAD=4 WORKING_CHUNK_SIZE=8388608
./em_algorithm_parallel_ubuntu.sh <input_folder>
```

For a large input:

```
make NUM_THREAD=32 WORKING_CHUNK_SIZE=1048576
```

The output ranks are stored in the `ranks` folder.