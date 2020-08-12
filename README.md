# SUFFIX RANK
## Suffix sorting for large inputs

### To Run
```
cd version_1.1
make
./em_algorithm_ubuntu.sh <input folder>
```
On MacOS, run
```
./em_algorithm_mac.sh <input folder>
```
instead.

### Change Memory Usage

Modify line 7 of Makefile, replacing 33554432.  Note that the algorithm will use 32 times this amount of RAM, so with the default chunk size of 33554432, it will use 1GB of RAM.  Chunk size must be a power of 2.

Chunk size can also be modified in utils.h, if you do not want to compile using Makefile.

### Test Correctness

```
./em_algorithm_ubuntu.sh <input folder>
cd ../ranks_to_sa_1.0
make
./ranks_to_sa_ubuntu.sh ../version_1.1/ranks ../version_1.1/output
cd ../correctness_test
./test_correctness.sh ../version_1.1/input ../version_1.1/output
```
Note that the correctness test takes a long time to run.
