#ifndef FWDPP_PERF_SINGLEPOP_T_HPP
#define FWDPP_PERF_SINGLEPOP_T_HPP

#include <chrono>
#include <fwdpp/diploid.hh>
#include <fwdpp/sugar.hpp>

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
  using singlepop_gamete_t = KTfwd::gamete;
#ifdef USE_BOOST
  template<typename mtype> using singlepop_gvector_t = boost::container::vector<singlepop_gamete_t, allocator_t<singlepop_gamete_t > >;
#else
  template<typename mtype> using singlepop_gvector_t = std::vector<singlepop_gamete_t, allocator_t<singlepop_gamete_t > >;
#endif
  template<typename mtype> using diploid_t = std::pair<std::size_t,std::size_t>;
}

struct singlepop_t : public KTfwd::sugar::singlepop<KTfwd::popgenmut,
						    this_program::singlepop_mvector_t<KTfwd::popgenmut>,
						    this_program::singlepop_gvector_t<KTfwd::popgenmut>,
						    std::vector< this_program::diploid_t<KTfwd::popgenmut> >,
						    std::vector<KTfwd::popgenmut>,
						    std::vector<KTfwd::uint_t>,
						    std::unordered_set<double,std::hash<double>,KTfwd::equal_eps> >
{
  using base_t = KTfwd::sugar::singlepop<KTfwd::popgenmut,
					 this_program::singlepop_mvector_t<KTfwd::popgenmut>,
					 this_program::singlepop_gvector_t<KTfwd::popgenmut>,
					 std::vector< this_program::diploid_t<KTfwd::popgenmut> >,
					 std::vector<KTfwd::popgenmut>,
					 std::vector<KTfwd::uint_t>,
					 std::unordered_set<double,std::hash<double>,KTfwd::equal_eps> >;
  std::chrono::time_point<std::chrono::system_clock> start, end;
  singlepop_t(KTfwd::uint_t N) : base_t(N)
  {
  }
};

#endif