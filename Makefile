B1;2cCXX=c++
CXXFLAGS=-I. -std=c++11 -O2 -Wall -W -DNDEBUG -pthread
GSL=-lgsl -lgslcblas
TCM=-ltcmalloc
TBBPRX=-ltbbmalloc_proxy
TBB=-ltbb -ltbbmalloc

all: std.o tbb_scalable.o tbb_allocator.o tbb_caa.o boost_fast_pool.o
	$(CXX) -o std std.o -pthread $(LDFLAGS) $(GSL)
	$(CXX) -o std_tcmalloc std.o -pthread $(LDFLAGS) $(GSL) $(TCM)
	$(CXX) -o std_tbb std.o -pthread $(LDFLAGS) $(GSL) $(TBBPRX)
	$(CXX) -o tbb_scalable tbb_scalable.o -pthread $(LDFLAGS) $(GSL) $(TBB)
	$(CXX) -o tbb_allocator tbb_allocator.o -pthread $(LDFLAGS) $(GSL) $(TBB)
	$(CXX) -o tbb_caa tbb_caa.o -pthread $(LDFLAGS) $(GSL) $(TBB)
	$(CXX) -o boost_fast_pool boost_fast_pool.o -pthread $(LDFLAGS) $(GSL) -lboost_system
clean:
	for i in $(shell find . -maxdepth 1 -type f -executable); do \
		rm -f $$i; \
	done
	rm -f *.o

EVOLVE=evolve.hpp

std.o: $(EVOLVE) simulate.cc
	$(CXX) $(CXXFLAGS) -o std.o -c simulate.cc
tbb_scalable.o: $(EVOLVE) simulate.cc
	$(CXX) -DUSE_INTEL_SCALABLE_ALLOCATOR $(CXXFLAGS) -o tbb_scalable.o -c simulate.cc
tbb_allocator.o: $(EVOLVE) simulate.cc
	$(CXX) -DUSE_INTEL_ALLOCATOR $(CXXFLAGS) -o tbb_allocator.o -c simulate.cc
tbb_caa.o: $(EVOLVE) simulate.cc
	$(CXX) -DUSE_INTEL_CACHE_ALIGNED_ALLOCATOR $(CXXFLAGS) -o tbb_caa.o -c simulate.cc
boost_fast_pool.o: $(EVOLVE) simulate.cc
	$(CXX) $(CXXFLAGS) -DUSE_BOOST -o boost_fast_pool.o -c simulate.cc
