#ifndef FWDPP_PERF_EVOLVE
#define FWDPP_PERF_EVOLVE

#include <fwdpp/diploid.hh>
#include <fwdpp/sugar.hpp>

template<typename T>
void evolve_t( T * pop,
	       unsigned long seed,
	       const uint_t * Nlist,
	       const size_t Nlist_len,
	       const double theta,
	       const double rho )
{
  using namespace KTfwd;
  locker.lock();
  std::cout << "I got seed " << seed << '\n';
  locker.unlock();
  GSLrng_t<GSL_RNG_MT19937> rng(seed);
  const double mu = theta/double(4*Nlist[0]),r=rho/double(4*Nlist[0]);
  pop->start = std::chrono::system_clock::now();
  for(size_t i =  0 ; i < Nlist_len ; ++i )
    {
      double wbar = sample_diploid(rng.get(),
				   pop->gametes, 
				   pop->diploids,
				   pop->mutations,
				   pop->mcounts,
				   Nlist[i], 
				   mu,   
				   std::bind(infsites(),std::placeholders::_1,std::placeholders::_2,rng.get(),std::ref(pop->mut_lookup),unsigned(i),
					     mu,0.,[&rng](){return gsl_rng_uniform(rng.get());},[](){return 0.;},[](){return 0.;}),
				   std::bind(KTfwd::poisson_xover(),rng.get(),r,0.,1.,
					     std::placeholders::_1,std::placeholders::_2,std::placeholders::_3),
				   std::bind(KTfwd::multiplicative_diploid(),std::placeholders::_1,std::placeholders::_2,
					     std::placeholders::_3,2.),
				   pop->neutral,pop->selected);
	KTfwd::update_mutations(pop->mutations,pop->fixations,pop->fixation_times,pop->mut_lookup,pop->mcounts,i,2*Nlist[i]);
    }
  pop->end = std::chrono::system_clock::now();
}

#endif
