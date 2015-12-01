# Performance testing

A multi-threaded simulation for benchmarking [fwdpp](http://gitub.com/molpopgen/fwdpp) with using different libraries providng malloc.

Purpose:

* Demonstrate how to use C++11 threads + fwdpp to run reproducible simulations in a multi-core environment
* Provide a quick way to benchmark the GCC memory allocators vs [tcmalloc](https://code.google.com/p/gperftools) and [tbb](https://www.threadingbuildingblocks.org) using the same set of code.

Requirements:

* GSL
* C++11 compiler
* fwdpp
* tcmalloc
* tbb

Compilation:

Install the requirements, then say "make".

There will never be a configure script or whatnot to check for dependencies.  These are advanced examples and tools that I use for quick performance testing.
