# SUFFIX RANK
## Suffix sorting for large inputs

### Running
```
cd version1
make
./em_algorithm_ubuntu.sh <input folder>
```
On MacOS, run
```
./em_algorithm_mac.sh <input folder>
```
instead.

### Changing Memory Used

Modify line 7 of Makefile, replacing 33554432.  Note that the algorithm will use 32 times this amount of RAM, so with the default chunk size of 33554432, it will use 1GB of RAM.  Chunk size must be a power of 2.

Chunk size can also be modified in utils.h, if you do not want to compile using Makefile.

### Testing Correctness

```
./em_algorithm_ubuntu.sh <input folder>
cd ../ranks_to_sa
make
./ranks_to_sa_ubuntu.sh ../version1/ranks ../version1/output
cd ../correctness_test
./test_corectness.sh ../version1/input ../version1/output
```
Note that the correctness test will take a long time.
