#include <future>
#include <memory>
#include <utility>
#include <iostream>
#include <functional>
#include <cmath>
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

  vector<std::future<shared_ptr<singlepop_t>> > futures;

  GSLrng_t<GSL_RNG_MT19937> rng(101);
  std::vector<uint_t> Nlist(10*N+1,N);
  //const std::size_t ES = std::log(2*N)*theta + (2./3.)*theta;
  for(uint_t i=0;i<nt;++i)
    {
      auto f = std::async(std::launch::async,evolve_async,gsl_rng_get(rng.get()),&Nlist[0],Nlist.size(),theta,rho);
      futures.emplace_back(std::move(f));
    }
  std::vector<std::shared_ptr<singlepop_t> > pops;
  for(auto & f : futures) pops.emplace_back(f.get());

  for(uint_t i=0;i<nt;++i)
    {
      chrono::duration<double> elapsed_seconds = pops[i]->end-pops[i]->start;
      cout << elapsed_seconds.count() << '\n';
    }
}
