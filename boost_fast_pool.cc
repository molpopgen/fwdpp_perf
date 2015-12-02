#include <thread>
#include <memory>
#include <utility>
#include <iostream>
#include <functional>
#include <mutex>
#include <fwdpp/diploid.hh>
#define FWDPP_SUGAR_USE_BOOST
#include <fwdpp/sugar.hpp>

using namespace KTfwd;

using singlepop_t = KTfwd::singlepop_serialized<KTfwd::popgenmut,
						KTfwd::mutation_writer,
						KTfwd::mutation_reader<KTfwd::popgenmut> >;

std::mutex locker;

#include <evolve.hpp>

int main(int argc, char **argv)
{
  int argn=1;
  const uint_t N = atoi(argv[argn++]);
  const unsigned nt = atoi(argv[argn++]);
  const double theta = atof(argv[argn++]);
  const double rho = atof(argv[argn++]);

  std::vector<std::shared_ptr<singlepop_t> > pops;
  std::vector<std::thread> threads;
  GSLrng_t<GSL_RNG_MT19937> rng(101);
  std::vector<uint_t> Nlist(10*N,N);
  for(uint_t i=0;i<nt;++i)
    {
      pops.emplace_back(std::make_shared<::singlepop_t>(singlepop_t(N)));
      threads.emplace_back( std::thread( evolve_t<singlepop_t>,pops[i].get(), gsl_rng_get(rng.get()),&Nlist[0],Nlist.size(),theta,rho) );
    }

  for(uint_t i=0;i<nt;++i) threads[i].join();
}

