B1;2cCXX=c++
CXXFLAGS=-I. -std=c++11 -O2 -Wall -W -DNDEBUG -pthread
GSL=-lgsl -lgslcblas
TCM=-ltcmalloc
TBBPRX=-ltbbmalloc_proxy
TBB=-ltbb -ltbbmalloc

all: std.o tbb_scalable.o tbb_allocator.o tbb_caa.o boost_fast_pool.o
	$(CXX) -o std std.o -pthread $(GSL)
	$(CXX) -o std_tcmalloc std.o -pthread $(GSL) $(TCM)
	$(CXX) -o std_tbb std.o -pthread $(GSL) $(TBBPRX)
	$(CXX) -o tbb_scalable tbb_scalable.o -pthread $(GSL) $(TBB)
	$(CXX) -o tbb_allocator tbb_allocator.o -pthread $(GSL) $(TBB)
	$(CXX) -o tbb_caa tbb_caa.o -pthread $(GSL) $(TBB)
	$(CXX) -o boost_fast_pool boost_fast_pool.o -pthread $(GSL) -lboost_system
clean:
	for i in $(shell find . -maxdepth 1 -type f -executable); do \
		rm -f $$i; \
	done
	rm -f *.o

EVOLVE=evolve.hpp

std.o: $(EVOLVE)
tbb_scalable.o: $(EVOLVE)
tbb_allocator.o: $(EVOLVE)
tbb_caa.o: $(EVOLVE)
boost_fast_pool.o: $(EVOLVE)
