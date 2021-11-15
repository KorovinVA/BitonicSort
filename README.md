# BitonicSort
Implementation of bitonic sort with SYCL 1.2.1

## Requirements
- Cmake version 3.10 or higher
- ~10 GB of free space (for generated tests)
### Windows
- Microsoft Visual Studio 2019
- Intel® oneAPI Base Toolkit

## Installation
The following rules are common to all systems:
```
$ git clone https://github.com/KorovinVA/BitonicSort.git
$ cd BitonicSort
$ mkdir build && cd build
```
### Windows
Only oneAPI DPC++ is supported for the windows target.
```
> cmake .. -G  "Visual Studio 16 2019" -T "Intel(R) oneAPI DPC++ Compiler"
> cmake --build .
```
### Linux
Define a compiler and compiler flags before build. For example:
```
$ cmake .. -DCMAKE_CXX_COMPILER=~/sycl_workspace/llvm/build/bin/clang++ -DCMAKE_CXX_FLAGS="-fsycl"
$ cmake --build .
```

Note. If you use [DPC++ toolchain](https://github.com/intel/llvm), LD_LIBRARY_PATH should be set:
```
$ export LD_LIBRARY_PATH=~/sycl_workspace/llvm/build/lib:LD_LIBRARY_PATH
```
## Usage
All tests and `bitonic_list` will be located in `tests` directory.
Pass one of the following arguments to the Bitonic Sort:
- -generate ---- Generate tests.
- -run-all ---- Run all generated tests which are specified in `bitonic_list`.
- -run=%testname% ---- Run %testname% (%testname% should exist in `tests`).
- -platforminfo ---- Show sycl available targets.
