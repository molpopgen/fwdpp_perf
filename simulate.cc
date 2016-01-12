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

#ifdef USE_BOOST
#include <boost/container/vector.hpp>
#include <boost/pool/pool_alloc.hpp>
template<typename T> using allocator_t = boost::fast_pool_allocator<T>;
#elif defined USE_INTEL_ALLOCATOR
#include <tbb/tbb_allocator.h>
template<typename T> using allocator_t = tbb::tbb_allocator<T>;
#elif defined USE_INTEL_SCALABLE_ALLOCATOR
#include <tbb/scalable_allocator.h>
template<typename T> using allocator_t = tbb::scalable_allocator<T>;
#elif defined USE_INTEL_CACHE_ALIGNED_ALLOCATOR
#include <tbb/cache_aligned_allocator.h>
template<typename T> using allocator_t = tbb::cache_aligned_allocator<T>;
#else
#include <memory>
template<typename T> using allocator_t = std::allocator<T>;
#endif
namespace this_program {
#ifdef USE_BOOST
  template<typename mtype> using singlepop_mvector_t = boost::container::vector<mtype,allocator_t<mtype> >;
#else
  template<typename mtype> using singlepop_mvector_t = std::vector<mtype,allocator_t<mtype> >;
#endif
  using singlepop_gamete_t = gamete;
  //template<typename mtype> using singlepop_gamete_t = gamete_base<mtype,singlepop_mvector_t<mtype> >;
#ifdef USE_BOOST
  template<typename mtype> using singlepop_gvector_t = boost::container::vector<singlepop_gamete_t, allocator_t<singlepop_gamete_t > >;
#else
  template<typename mtype> using singlepop_gvector_t = std::vector<singlepop_gamete_t, allocator_t<singlepop_gamete_t > >;
#endif
  template<typename mtype> using diploid_t = std::pair<std::size_t,std::size_t>;
}

struct singlepop_t : public KTfwd::sugar::singlepop<KTfwd::popgenmut,
						    this_program::singlepop_mvector_t<popgenmut>,
						    this_program::singlepop_gvector_t<popgenmut>,
						    vector< this_program::diploid_t<popgenmut> >,
						    vector<popgenmut>,
						    vector<uint_t>,
						    std::unordered_set<double,std::hash<double>,KTfwd::equal_eps> >
{
  using base_t = KTfwd::sugar::singlepop<KTfwd::popgenmut,
					 this_program::singlepop_mvector_t<popgenmut>,
					 this_program::singlepop_gvector_t<popgenmut>,
					 vector< this_program::diploid_t<popgenmut> >,
					 vector<popgenmut>,
					 vector<uint_t>,
					 std::unordered_set<double,std::hash<double>,KTfwd::equal_eps> >;
  std::chrono::time_point<std::chrono::system_clock> start, end;
  singlepop_t(KTfwd::uint_t N) : base_t(N)
  {
  }
};
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
  for(uint_t i=0;i<nt;++i)
    {
      chrono::duration<double> elapsed_seconds = pops[i]->end-pops[i]->start;
      cout << elapsed_seconds.count() << '\n';
    }
}
