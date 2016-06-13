// GRS OpenMP parallel sparse CSR matrix-matrix multiplication variant.
// The implementation is based on a new(?) SpMM_DSParallel_RowStore
// algorithm by Anders Gidenstam, inspired by the classical sequential
// algorithms in [Gustavson, ACM TMS 4(3), 1978] and
// [Gilbert et al., SIAM JMAA 13(1), 1992].
//
// Copyright (C) 2016  Anders Gidenstam
//
// This program is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "SpGEMM_variant_DSParallel.h"

#include <ctime>

#include <SpGEMM_DSParallel.h>
#include <EXCESS/concurrent_bag>

namespace EXCESS_GRS {

static void   perform_op(int argcin, int argcout,
                         grs_data **argvin, grs_data **argvout);
static double predict_cost(int argcin, int *in_state, int *in_size);
static double predict_switch(int argcin, int *in_state, int *in_size);

SpGEMM_variant_csr_dsparallel::
  SpGEMM_variant_csr_dsparallel(struct grs_component* component, int variant)
{
  grs_component_add_variant(component, &this_variant);
  grs_variant_init(&this_variant, perform_op, variant,
                   predict_cost, predict_switch);
}

SpGEMM_variant_csr_dsparallel::~SpGEMM_variant_csr_dsparallel()
{
  grs_variant_destroy(&this_variant);
}

static void perform_op(int argcin, int argcout,
                       grs_data **argvin, grs_data **argvout)
{
  std::cout << "SpGEMM_variant_dsparallel::perform_op started." << std::endl;
  if (argcin != 8 || argcout != 4) {
    std::cout << ("SpGEMM_variant_dsparallel::perform_op: Error: "
                  "Wrong number of arguments.")
              << std::endl;
    return;
  }
  SpMatrix A(0,0,0), B(0,0,0);
  matrix_csr::convert_from_grs_input(argvin,     A);
  matrix_csr::convert_from_grs_input(argvin + 4, B);

  {
    struct timespec t1, t2; 
    std::cout << "Attempting matrix matrix multiplication "
              << (A.m) << "x" << (A.n) << " * "
              << (B.m) << "x" << (B.n) << " ... ";

    clock_gettime(CLOCK_REALTIME, &t1);
    SpMatrix C =
      SpMM_DSParallel_RowStore<excess::concurrent_bag_MSTLB>(A, B);
    clock_gettime(CLOCK_REALTIME, &t2);

    std::cout << "Ok." << std::endl;
    std::cout << "% Result C is " << (C.m) << "x" << (C.n) << " matrix with "
              << (C.nzmax) << " non-zeros." << std::endl;
    std::cout << "% Duration: "
              << ((double)(t2.tv_sec - t1.tv_sec) +
                  1e-9 * (double)(t2.tv_nsec - t1.tv_nsec))
              << " sec" << std::endl;
    matrix_csr::convert_to_grs_output(C, argvout);
  }
  matrix_csr::clear(A);
  matrix_csr::clear(B);

  std::cout << "SpGEMM_variant_dsparallel::perform_op finished." << std::endl;
}

static double predict_cost(int argcin, int *in_state, int *in_size)
{
  // FIXME.
  return 4.0;
}

static double predict_switch(int argcin, int *in_state, int *in_size)
{
  // FIXME.
  return 0.0;
}

}
