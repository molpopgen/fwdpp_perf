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
  auto recmap = [&rng](){return gsl_rng_uniform(rng.get());};
  const double mu = theta/double(4*Nlist[0]),r=rho/double(4*Nlist[0]);
  pop->start = std::chrono::system_clock::now();
  for(size_t i =  0 ; i < Nlist_len ; ++i )
    {
      double wbar = sample_diploid(rng.get(),
				   &pop->gametes, 
				   &pop->diploids,
				   &pop->mutations,
				   Nlist[i], 
				   mu,   
				   std::bind(infsites(),std::placeholders::_1,std::placeholders::_2,rng.get(),&pop->mut_lookup,unsigned(i),
					     mu,0.,[&rng](){return gsl_rng_uniform(rng.get());},[](){return 0.;},[](){return 0.;}),
				   std::bind(KTfwd::genetics101(),std::placeholders::_1,std::placeholders::_2,
					     std::placeholders::_3,
					     std::placeholders::_4,
					     //Pass as reference
					     std::ref(pop->neutral),std::ref(pop->selected),
					     &pop->gametes,
					     r,
					     rng.get(),
					     recmap),
				   std::bind(KTfwd::insert_at_end<singlepop_t::gamete_t,singlepop_t::glist_t>,std::placeholders::_1,std::placeholders::_2),
				   std::bind(KTfwd::multiplicative_diploid(),std::placeholders::_1,std::placeholders::_2,2.),
				   std::bind(KTfwd::mutation_remover(),std::placeholders::_1,0,2*Nlist[i]));
      	  KTfwd::remove_fixed_lost(&pop->mutations,&pop->fixations,&pop->fixation_times,&pop->mut_lookup,i,2*Nlist[i]);
    }
  pop->end = std::chrono::system_clock::now();
}

#endif
