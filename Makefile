B1;2cCXX=c++
CXXFLAGS=-I. -std=c++11 -O2 -Wall -W -DNDEBUG -pthread
GSL=-lgsl -lgslcblas
TCM=-ltcmalloc
TBBPRX=-ltbbmalloc_proxy
TBB=-ltbb -ltbbmalloc
all: std.o tbb_scalable.o tbb_allocator.o tbb_caa.o
	$(CXX) -o std std.o -pthread $(GSL)
	$(CXX) -o std_tcmalloc std.o -pthread $(GSL) $(TCM)
	$(CXX) -o std_tbb std.o -pthread $(GSL) $(TBBPRX)
	$(CXX) -o tbb_scalable tbb_scalable.o -pthread $(GSL) $(TBB)
	$(CXX) -o tbb_allocator tbb_allocator.o -pthread $(GSL) $(TBB)
	$(CXX) -o tbb_caa tbb_caa.o -pthread $(GSL) $(TBB)
clean:
	rm -f *.o
