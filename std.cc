#include <thread>
#include <memory>
#include <utility>
#include <iostream>
#include <functional>
#include <mutex>
#include <fwdpp/diploid.hh>
#include <fwdpp/sugar.hpp>

using namespace std;
using namespace KTfwd;

using singlepop_t = KTfwd::singlepop_serialized<KTfwd::popgenmut,
						KTfwd::mutation_writer,
						KTfwd::mutation_reader<KTfwd::popgenmut> >;

mutex locker;

template<typename T>
void evolve_t( T * pop,
	       unsigned long seed,
	       const uint_t * Nlist,
	       const size_t Nlist_len,
	       const double theta,
	       const double rho )
{
  locker.lock();
  cout << "I got seed " << seed << '\n';
  locker.unlock();
  GSLrng_t<GSL_RNG_MT19937> rng(seed);
  auto recmap = [&rng](){return gsl_rng_uniform(rng.get());};
  const double mu = theta/double(4*Nlist[0]),r=rho/double(4*Nlist[0]);
  for(size_t i =  0 ; i < Nlist_len ; ++i )
    {
      double wbar = sample_diploid(rng.get(),
				   &pop->gametes, 
				   &pop->diploids,
				   &pop->mutations,
				   Nlist[i], 
				   mu,   
				   std::bind(infsites(),rng.get(),&pop->mut_lookup,unsigned(i),
					     mu,0.,[&rng](){return gsl_rng_uniform(rng.get());},[](){return 0.;},[](){return 0.;}),
				   std::bind(KTfwd::genetics101(),std::placeholders::_1,std::placeholders::_2,
					     std::placeholders::_3,
					     //Pass as reference
					     std::ref(pop->neutral),std::ref(pop->selected),
					     &pop->gametes,
					     r,
					     rng.get(),
					     recmap),
				   std::bind(KTfwd::insert_at_end<singlepop_t::mutation_t,singlepop_t::mlist_t>,std::placeholders::_1,std::placeholders::_2),
				   std::bind(KTfwd::insert_at_end<singlepop_t::gamete_t,singlepop_t::glist_t>,std::placeholders::_1,std::placeholders::_2),
				   std::bind(KTfwd::multiplicative_diploid(),std::placeholders::_1,std::placeholders::_2,2.),
				   std::bind(KTfwd::mutation_remover(),std::placeholders::_1,0,2*Nlist[i]));
      	  KTfwd::remove_fixed_lost(&pop->mutations,&pop->fixations,&pop->fixation_times,&pop->mut_lookup,i,2*Nlist[i]);
    }
}

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
      threads.emplace_back( thread( evolve_t<singlepop_t>,pops[i].get(), gsl_rng_get(rng.get()),&Nlist[0],Nlist.size(),theta,rho) );
    }

  for(uint_t i=0;i<nt;++i) threads[i].join();
}

