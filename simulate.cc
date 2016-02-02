#include <thread>
#include <memory>
#include <utility>
#include <iostream>
#include <functional>
#include <fwdpp/diploid.hh>
#include <fwdpp/sugar.hpp>
#include <evolve.hpp>

using namespace std;
using namespace KTfwd;



int main(int argc, char **argv)
{
  int argn=1;
  const uint_t N = atoi(argv[argn++]);
  const unsigned nt = atoi(argv[argn++]);
  const double theta = atof(argv[argn++]);
  const double rho = atof(argv[argn++]);

  vector<shared_ptr<singlepop_t> > pops;
  vector<thread> threads;
  GSLrng_t<GSL_RNG_MT19937> rng(101);
  std::vector<uint_t> Nlist(10*N,N);
  for(uint_t i=0;i<nt;++i)
    {
      pops.emplace_back(make_shared<::singlepop_t>(singlepop_t(N)));
      threads.emplace_back( thread( evolve_t,pops[i].get(), gsl_rng_get(rng.get()),&Nlist[0],Nlist.size(),theta,rho) );
    }

  for(uint_t i=0;i<nt;++i) threads[i].join();
  for(uint_t i=0;i<nt;++i)
    {
      chrono::duration<double> elapsed_seconds = pops[i]->end-pops[i]->start;
      cout << elapsed_seconds.count() << '\n';
    }
}
