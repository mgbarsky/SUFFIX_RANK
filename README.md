# SUFFIX RANK
## Suffix sorting for large inputs


Installation instructions for SAScan algorithm:
Setup system: <br>
yum -y install cmake <br>
yum -y install gcc <br>
yum -y install gcc-c++ <br>

Download SAscan:
1. download from: https://www.cs.helsinki.fi/group/pads/SAscan.html
2. unzip using tar xjf file.tar.bz2

Get Divsufsort
1.  Clone or download libdivsufsort from https://github.com/y-256/libdivsufsort (not the link given in the README of SAscan)
2.  Make a build directory and run cmake: <br>
	cd libdivsufsort <br>
	mkdir build <br>
	cd build <br>
	ccmake .. 
3.  Press c
4.  Make the following changes: <br>
	set BUILD_SHARED_LIBS to OFF <br>
	set BUILD_DIVSUFSORT64 to ON <br>
	set CMAKE_BUILD_TYPE to Release 
5.  Press c, then g
6.  Install: <br>
	make <br>
	sudo make install
7. Now move to SAscan and try to run it: <br>
	cd SAscan-0.1.1/src <br>
	make <br>
	./sascan <input_file>
