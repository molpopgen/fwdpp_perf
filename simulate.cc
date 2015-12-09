#include <thread>
#include <memory>
#include <utility>
#include <iostream>
#include <functional>
#include <mutex>
#include <fwdpp/diploid.hh>
#ifdef USE_BOOST
#define FWDPP_SUGAR_USE_BOOST
#endif
#include <fwdpp/sugar.hpp>

using namespace std;
using namespace KTfwd;

#ifdef USE_INTEL_ALLOCATOR
#include <tbb/tbb_allocator.h>
namespace this_program {
  template<typename mtype> using singlepop_mlist_t = std::list<mtype,tbb::tbb_allocator<mtype> >;
  template<typename mtype> using singlepop_gamete_t = gamete_base<mtype, singlepop_mlist_t<mtype> >;
  template<typename mtype> using singlepop_glist_t = std::list<singlepop_gamete_t<mtype>,
							       tbb::tbb_allocator<singlepop_gamete_t<mtype> > >;
  template<typename mtype> using diploid_t = std::pair<typename singlepop_glist_t<mtype>::iterator,
						       typename singlepop_glist_t<mtype>::iterator>;
}

using singlepop_t = KTfwd::sugar::singlepop_serialized<KTfwd::popgenmut,
						       KTfwd::mutation_writer,
						       KTfwd::mutation_reader<KTfwd::popgenmut>,
						       this_program::singlepop_mlist_t<popgenmut>,
						       this_program::singlepop_glist_t<popgenmut>,
						       vector< this_program::diploid_t<popgenmut> >,
						       vector<popgenmut>,
						       vector<uint_t>,
						       std::unordered_set<double,std::hash<double>,KTfwd::equal_eps> >;
#elif defined USE_INTEL_SCALABLE_ALLOCATOR
#include <tbb/scalable_allocator.h>
namespace this_program {
  template<typename mtype> using singlepop_mlist_t = std::list<mtype,tbb::scalable_allocator<mtype> >;
  template<typename mtype> using singlepop_gamete_t = gamete_base<mtype, singlepop_mlist_t<mtype> >;
  template<typename mtype> using singlepop_glist_t = std::list<singlepop_gamete_t<mtype>,
							       tbb::scalable_allocator<singlepop_gamete_t<mtype> > >;
  template<typename mtype> using diploid_t = std::pair<typename singlepop_glist_t<mtype>::iterator,
						       typename singlepop_glist_t<mtype>::iterator>;
}

using singlepop_t = KTfwd::sugar::singlepop_serialized<KTfwd::popgenmut,
						       KTfwd::mutation_writer,
						       KTfwd::mutation_reader<KTfwd::popgenmut>,
						       this_program::singlepop_mlist_t<popgenmut>,
						       this_program::singlepop_glist_t<popgenmut>,
						       vector< this_program::diploid_t<popgenmut> >,
						       vector<popgenmut>,
						       vector<uint_t>,
						       std::unordered_set<double,std::hash<double>,KTfwd::equal_eps> >;
#elif defined USE_INTEL_CACHE_ALIGNED_ALLOCATOR
#include <tbb/cache_aligned_allocator.h>
namespace this_program {
  template<typename mtype> using singlepop_mlist_t = std::list<mtype,tbb::cache_aligned_allocator<mtype> >;
  template<typename mtype> using singlepop_gamete_t = gamete_base<mtype, singlepop_mlist_t<mtype> >;
  template<typename mtype> using singlepop_glist_t = std::list<singlepop_gamete_t<mtype>,
							       tbb::cache_aligned_allocator<singlepop_gamete_t<mtype> > >;
  template<typename mtype> using diploid_t = std::pair<typename singlepop_glist_t<mtype>::iterator,
						       typename singlepop_glist_t<mtype>::iterator>;
}

using singlepop_t = KTfwd::sugar::singlepop_serialized<KTfwd::popgenmut,
						       KTfwd::mutation_writer,
						       KTfwd::mutation_reader<KTfwd::popgenmut>,
						       this_program::singlepop_mlist_t<popgenmut>,
						       this_program::singlepop_glist_t<popgenmut>,
						       vector< this_program::diploid_t<popgenmut> >,
						       vector<popgenmut>,
						       vector<uint_t>,
						       std::unordered_set<double,std::hash<double>,KTfwd::equal_eps> >;
#else
using singlepop_t = KTfwd::singlepop_serialized<KTfwd::popgenmut,
						KTfwd::mutation_writer,
						KTfwd::mutation_reader<KTfwd::popgenmut> >;
#endif

mutex locker;

#include <evolve.hpp>

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

