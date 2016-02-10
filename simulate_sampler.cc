#include <thread>
#include <memory>
#include <utility>
#include <iostream>
#include <functional>
#include <cmath>
#include <future>
#include <fwdpp/diploid.hh>
#include <fwdpp/sugar.hpp>
#include <fwdpp/sugar/sampling.hpp>
#include <evolve.hpp>

using namespace std;
using namespace KTfwd;

struct sampler
{
  unsigned nsam;
  using result_type = KTfwd::sep_sample_t;
  inline result_type operator()(const singlepop_t * pop,gsl_rng * r) const
  {
    return KTfwd::sample_separate(r,*pop,nsam,true);
  }
};


int main(int argc, char **argv)
{
  int argn=1;
  const uint_t N = atoi(argv[argn++]);
  const unsigned nt = atoi(argv[argn++]);
  const double theta = atof(argv[argn++]);
  const double rho = atof(argv[argn++]);

  vector<shared_ptr<singlepop_t> > pops;
  using future_t = std::future<vector<pair<unsigned,sampler::result_type> > >;
  vector< future_t > futures;
  GSLrng_t<GSL_RNG_MT19937> rng(101);
  std::vector<uint_t> Nlist(10*N,N);
  const std::size_t ES = std::log(2*N)*theta + (2./3.)*theta;
  sampler s;
  s.nsam=10;
  for(uint_t i=0;i<nt;++i)
    {
      pops.emplace_back(make_shared<::singlepop_t>(singlepop_t(N)));
      pops[i]->mutations.reserve(ES);
      futures.emplace_back( async(launch::async,evolve_sample_t<sampler>,
				  pops[i].get(),
				  gsl_rng_get(rng.get()),
				  &Nlist[0],
				  Nlist.size(),
				  theta,
				  rho,
				  s,
				  10) );
    }

  vector<vector<pair<unsigned,sampler::result_type> > >results(futures.size());
  for(unsigned i=0;i<futures.size();++i) results[i]=futures[i].get();
  for( const auto & pop : pops)
    {
      chrono::duration<double> elapsed_seconds = pop->end-pop->start;
      cout << elapsed_seconds.count() << '\n';
    }
}
