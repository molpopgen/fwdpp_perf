/*
  Example of using MPI to parallelize multiple simulation runs.
  
  fwdpp's serialization routines are used to facilitate MPI_Send/MPI_Recv operations
*/
#include <mpi.h>
#include <thread>
#include <memory>
#include <utility>
#include <iostream>
#include <functional>
#include <mutex>
#include <chrono>
#include <fwdpp/diploid.hh>
#include <fwdpp/sugar.hpp>

using namespace std;
using namespace KTfwd;

using mwriter = KTfwd::mutation_writer;
using mreader = KTfwd::mutation_reader<popgenmut>;

struct singlepop_t : public KTfwd::singlepop<KTfwd::popgenmut>
						    
{
  using base_t = KTfwd::singlepop<KTfwd::popgenmut>;
  std::chrono::time_point<std::chrono::system_clock> start, end;
  singlepop_t(KTfwd::uint_t N) : base_t(N)
  {
  }
  singlepop_t(singlepop_t &&) = default;
};

mutex locker;

#include <evolve.hpp>

singlepop_t evolve_it( const uint_t N, const double theta, const double rho, const unsigned long seed )
{
  singlepop_t pop(N);
  std::vector<uint_t> nlist(10*N,N);
  evolve_t(&pop,seed,nlist.data(),nlist.size(),theta,rho);
  return pop;
}

int main(int argc, char **argv)
{
  int argn=1;
  const uint_t N = atoi(argv[argn++]);
  const double theta = atof(argv[argn++]);
  const double rho = atof(argv[argn++]);
  int rank=0,nt=0,provided=0;

  vector<shared_ptr<singlepop_t> > pops;
  GSLrng_t<GSL_RNG_MT19937> rng(101);
  MPI_Init_thread(&argc,&argv,MPI_THREAD_SERIALIZED,&provided);
  MPI_Comm_size(MPI_COMM_WORLD,&nt);

  MPI_Comm_rank(MPI_COMM_WORLD,&rank);

  /*
    rank 0 assigns seeds and sends to other processes
    It then receives data from other processes, and uses
    serialization to construct a singlepop_t and add it to pops.
    
    The other ranks do the work
  */
  unsigned long seed;
  std::uint64_t size;
  if( rank == 0 )
    {
      //generate seeds
      for(int i=1;i<nt;++i)
      	{
	  //std::cerr << "getting seed for rank " << i << ": ";
	  seed = gsl_rng_get(rng.get());
	  //std::cerr << "sending seed " << seed << '\n';
	  MPI_Send(&seed,1,MPI_UNSIGNED_LONG,i,0,MPI_COMM_WORLD);	 
	}
      //Get data from "worker" processes
      for(int i=1;i<nt;++i)
	{
	  MPI_Recv(&size,i,MPI_UINT64_T,i,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	  unique_ptr<char[]> buffer(new char[size]);
	  MPI_Recv(buffer.get(),size,MPI_CHAR,i,2,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	  KTfwd::serialize s;
	  s.buffer.str(std::string(buffer.get(),buffer.get()+size));
	  singlepop_t temp(0);
	  KTfwd::deserialize()(temp,s,mreader());
	  pops.emplace_back(make_shared<singlepop_t>(std::move(temp)));
	}
    }
  else
    {
      //Get seed from rank 0
      MPI_Recv(&seed,1,MPI_UNSIGNED_LONG,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      //get an evolved pop
      auto pop = evolve_it(N,theta,rho,seed);
      //serialize population to send back to rank 0 
      KTfwd::serialize s;
      s(pop,mwriter());
      size = s.buffer.str().size();
      MPI_Send(&size,1,MPI_UINT64_T,0,1,MPI_COMM_WORLD);
      const char * b = s.buffer.str().c_str();
      MPI_Send(b,size,MPI_CHAR,0,2,MPI_COMM_WORLD);
    }
  
  MPI_Finalize();

  for(std::size_t i=0;i<pops.size();++i)
    {
      std::cout << pops[i]->N << ' ' << pops[i]->mutations.size() << ' ' << pops[i]->gametes.size() << '\n';
    }


  // for(uint_t i=0;i<nt;++i) threads[i].join();

}
