#include <evolve.hpp>
#include <mutex>
#include <chrono>

std::mutex locker;

void evolve_t( singlepop_t * pop,
	       unsigned long seed,
	       const KTfwd::uint_t * Nlist,
	       const size_t Nlist_len,
	       const double theta,
	       const double rho )
{
  using namespace KTfwd;
  using namespace std;
  locker.lock();

  locker.unlock();
  GSLrng_t<GSL_RNG_MT19937> rng(seed);
  const double mu = theta/double(4*Nlist[0]),r=rho/double(4*Nlist[0]);
  pop->start = std::chrono::system_clock::now();
  unsigned i=0;
  const auto mmodel=std::bind(infsites(),std::placeholders::_1,std::placeholders::_2,rng.get(),std::ref(pop->mut_lookup),&i,
			mu,0.,[&rng](){return gsl_rng_uniform(rng.get());},[](){return 0.;},[](){return 0.;});
  const auto recmodel = std::bind(KTfwd::poisson_xover(),rng.get(),r,0.,1.,
				  std::placeholders::_1,std::placeholders::_2,std::placeholders::_3);
  const auto fitness_model = std::bind(KTfwd::multiplicative_diploid(),std::placeholders::_1,std::placeholders::_2,
				       std::placeholders::_3,2.);
  const std::vector<KTfwd::uint_t> Ns(Nlist,Nlist+Nlist_len);

  for(const auto N : Ns)
    {
      double wbar = sample_diploid(rng.get(),
				   pop->gametes,
				   pop->diploids,
				   pop->mutations,
				   pop->mcounts,
				   N,
				   mu,
				   mmodel,recmodel,fitness_model,
				   pop->neutral,pop->selected);
      KTfwd::update_mutations(pop->mutations,pop->fixations,pop->fixation_times,pop->mut_lookup,pop->mcounts,i,2*N);
      ++i;
    }
  pop->end = std::chrono::system_clock::now();
}

std::shared_ptr<singlepop_t> evolve_async(unsigned long seed,
					  const KTfwd::uint_t * Nlist,
					  const size_t Nlist_len,
					  const double theta,
					  const double rho )
{
  using namespace KTfwd;
  using namespace std;
  locker.lock();

  locker.unlock();
  GSLrng_t<GSL_RNG_MT19937> rng(seed);
  const double mu = theta/double(4*Nlist[0]),r=rho/double(4*Nlist[0]);
  singlepop_t pop(Nlist[0]);
  pop.mutations.reserve( std::size_t( std::log(2*pop.N)*theta + (2./3.)*(theta) ) );
  pop.start = std::chrono::system_clock::now();
  const std::vector<KTfwd::uint_t> Ns(Nlist,Nlist+Nlist_len);
  unsigned i=0;
  const auto mmodel=std::bind(infsites(),std::placeholders::_1,std::placeholders::_2,rng.get(),std::ref(pop.mut_lookup),&i,
			mu,0.,[&rng](){return gsl_rng_uniform(rng.get());},[](){return 0.;},[](){return 0.;});
  const auto recmodel = std::bind(KTfwd::poisson_xover(),rng.get(),r,0.,1.,
				  std::placeholders::_1,std::placeholders::_2,std::placeholders::_3);
  const auto fitness_model = std::bind(KTfwd::multiplicative_diploid(),std::placeholders::_1,std::placeholders::_2,
				       std::placeholders::_3,2.);
  for(const auto N : Ns)
    {
      double wbar = sample_diploid(rng.get(),
				   pop.gametes,
				   pop.diploids,
				   pop.mutations,
				   pop.mcounts,
				   N,
				   mu,
				   mmodel,recmodel,fitness_model,
				   pop.neutral,pop.selected);
      KTfwd::update_mutations(pop.mutations,pop.fixations,pop.fixation_times,pop.mut_lookup,pop.mcounts,i,2*N);
      ++i;
    }
  pop.end = std::chrono::system_clock::now();
  return std::make_shared<singlepop_t>(std::move(pop));
}
