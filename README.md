# Performance testing

A multi-threaded simulation for benchmarking [fwdpp](http://gitub.com/molpopgen/fwdpp) with using different libraries providng malloc.

## Purpose:

* Demonstrate how to use C++11 threads + fwdpp to run reproducible simulations in a multi-core environment
* Provide a quick way to benchmark the GCC memory allocators vs [tcmalloc](https://code.google.com/p/gperftools) and [tbb](https://www.threadingbuildingblocks.org) using the same set of code.

## Requirements:

* GSL
* C++11 compiler
* fwdpp
* tcmalloc
* tbb
* boost

## Compilation:

Install the requirements, then say "make".

There will never be a configure script or whatnot to check for dependencies.  These are advanced examples and tools that I use for quick performance testing.

## The programs

The executables compiled are a set of binaries that simuate a neutral Wright-Fisher model of $N$ diploids for $10N$ generations with parameters $\theta=4N\mu$ and $\rho=4Nr$.

Each executable is essentially the same code, in the sense that they all call the same "evolve" function with the same sets of seeds.  They differ in the use of internal memory allocators, external memory allocation libraries, etc.

The programs all generate the same output, because they use the same random number seed.
