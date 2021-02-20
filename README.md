# SUFFIX RANK
## Suffix sorting for large inputs

### To Run
```
cd src
make
./suffixrank.sh <input folder>
```

Sample input folder can be downloaded from [here](https://drive.google.com/file/d/1B9muEMI97aF8-Zj_SCxHzA1tMCjtNCbR/view).

### Change Memory Usage

Modify line 7 of Makefile, replacing 33554432.  Note that the algorithm will use 32 times this amount of RAM, so with the default chunk size of 33554432, it will use 1GB of RAM.  Chunk size must be a power of 2.

Chunk size can also be modified in utils.h, if you do not want to compile using Makefile.

### Test Correctness

```
cd correctness_test
./test_correctness.sh ../src/input ../src/output
```
Note that the correctness test takes a long time to run.
