CXX=c++
CXXFLAGS=$(CPPFLAGS) -I. -std=c++11 -O2 -Wall -W -DNDEBUG -pthread
GSL=-lgsl -lgslcblas
TCM=-ltcmalloc
TBBPRX=-ltbbmalloc_proxy
TBB=-ltbb -ltbbmalloc
MPILIBS=-lmpi

all: std tcmalloc tbb boost mpi

std: std.o std_evolve.o
	$(CXX) -o std std.o std_evolve.o -pthread $(LDFLAGS) $(GSL)

tcmalloc: std.o std_evolve.o
	$(CXX) -o std_tcmalloc std.o std_evolve.o -pthread $(LDFLAGS) $(GSL) $(TCM)

tbb: tbb_scalable.o tbb_allocator.o tbb_caa.o tbb_scalable_evolve.o tbb_caa_evolve.o tbb_allocator_evolve.o std.o std_evolve.o
	$(CXX) -o std_tbb std.o std_evolve.o -pthread $(LDFLAGS) $(GSL) $(TBBPRX)
	$(CXX) -o tbb_scalable tbb_scalable.o tbb_scalable_evolve.o -pthread $(LDFLAGS) $(GSL) $(TBB)
	$(CXX) -o tbb_allocator tbb_allocator.o tbb_allocator_evolve.o -pthread $(LDFLAGS) $(GSL) $(TBB)
	$(CXX) -o tbb_caa tbb_caa.o tbb_caa_evolve.o -pthread $(LDFLAGS) $(GSL) $(TBB)

boost: boost_fast_pool.o boost_evolve.o
	$(CXX) -o boost_fast_pool boost_fast_pool.o boost_evolve.o -pthread $(LDFLAGS) $(GSL) -lboost_system

mpi: neutral_mpi.o
	$(CXX) -o neutral_mpi neutral_mpi.o evolve.o $(LDFLAGS) $(GSL) $(MPILIBS)

clean:
	for i in $(shell find . -maxdepth 1 -type f -executable); do \
		rm -f $$i; \
	done
	rm -f *.o

EVOLVE=evolve.hpp

std.o: $(EVOLVE) simulate.cc
	$(CXX) $(CXXFLAGS) -o std.o -c simulate.cc
std_evolve.o: $(EVOLVE) evolve.cc
	$(CXX) $(CXXFLAGS) -o std_evolve.o -c evolve.cc

tbb_scalable.o: $(EVOLVE) simulate.cc
	$(CXX) -DUSE_INTEL_SCALABLE_ALLOCATOR $(CXXFLAGS) -o tbb_scalable.o -c simulate.cc
tbb_scalable_evolve.o: $(EVOLVE) evolve.cc
	$(CXX) -DUSE_INTEL_SCALABLE_ALLOCATOR $(CXXFLAGS) -o tbb_scalable_evolve.o -c evolve.cc
tbb_allocator.o: $(EVOLVE) simulate.cc
	$(CXX) -DUSE_INTEL_ALLOCATOR $(CXXFLAGS) -o tbb_allocator.o -c simulate.cc
tbb_caa.o: $(EVOLVE) simulate.cc
	$(CXX) -DUSE_INTEL_CACHE_ALIGNED_ALLOCATOR $(CXXFLAGS) -o tbb_caa.o -c simulate.cc
tbb_allocator_evolve.o: $(EVOLVE) evolve.cc
	$(CXX) -DUSE_INTEL_ALLOCATOR $(CXXFLAGS) -o tbb_allocator_evolve.o -c evolve.cc
tbb_caa_evolve.o: $(EVOLVE) evolve.cc
	$(CXX) -DUSE_INTEL_CACHE_ALIGNED_ALLOCATOR $(CXXFLAGS) -o tbb_caa_evolve.o -c evolve.cc
boost_fast_pool.o: $(EVOLVE) simulate.cc
	$(CXX) $(CXXFLAGS) -DUSE_BOOST -o boost_fast_pool.o -c simulate.cc
boost_evolve.o: $(EVOLVE) simulate.cc
	$(CXX) $(CXXFLAGS) -DUSE_BOOST -o boost_evolve.o -c evolve.cc
neutral_mpi.o: $(EVOLVE) neutral_mpi.cc
