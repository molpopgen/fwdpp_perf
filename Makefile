CXX=c++
CXXFLAGS=-std=c++11 -O2 -Wall -W -DNDEBUG -pthread
GSL=-lgsl -lgslcblas
TCM=-ltcmalloc
TBB=-ltbbmalloc_proxy
all: std.o
	$(CXX) -o std std.o -pthread $(GSL)
	$(CXX) -o std_tcmalloc std.o -pthread $(GSL) $(TCM)
	$(CXX) -o std_tbb std.o -pthread $(GSL) $(TBB)

clean:
	rm -f *.o
