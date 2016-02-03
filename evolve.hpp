#ifndef FWDPP_PERF_EVOLVE
#define FWDPP_PERF_EVOLVE

#include <singlepop_t.hpp>

void evolve_t( singlepop_t * pop,
	       unsigned long seed,
	       const KTfwd::uint_t * Nlist,
	       const size_t Nlist_len,
	       const double theta,
	       const double rho );

#endif
