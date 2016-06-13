// GRS GPU based sparse CSR matrix-matrix multiplication variant.
// The implementation depends on the bhSPARSE CSR implementation
// from https://github.com/bhSPARSE/Benchmark_SpGEMM_using_CSR
// (MIT License) published in
// [Weifeng Liu and Brian Vinter, "An Efficient GPU General Sparse
// Matrix-Matrix Multiplication for Irregular Data".
// Parallel and Distributed Processing Symposium, 2014 IEEE 28th International
// (IPDPS '14), pp.370-381, 19-23 May 2014.]
// and
// [Weifeng Liu and Brian Vinter, "A Framework for General Sparse
// Matrix-Matrix Multiplication on GPUs and Heterogeneous Processors". Journal
// of Parallel and Distributed Computing (JPDC), pp.47-61, Volume 85,
// November 2015.]
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
#ifndef __SPGEMM_GRS_VAR_BHSPARSE_CUDA
#define __SPGEMM_GRS_VAR_BHSPARSE_CUDA

#include "SpGEMM_component.h"

namespace EXCESS_GRS {

class SpGEMM_variant_csr_bhSPARSE_cuda : public SpGEMM_component_csr::variant
{
public:
  SpGEMM_variant_csr_bhSPARSE_cuda(struct grs_component* component,
                                   int variant);
  ~SpGEMM_variant_csr_bhSPARSE_cuda();

private:
  struct grs_variant this_variant;
};

}
#endif
