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
#ifndef __SPGEMM_GRS_VAR_DSPARALLEL
#define __SPGEMM_GRS_VAR_DSPARALLEL

#include "SpGEMM_component.h"

namespace EXCESS_GRS {

class SpGEMM_variant_csr_dsparallel : public SpGEMM_component_csr::variant
{
public:
  SpGEMM_variant_csr_dsparallel(struct grs_component* component, int variant);
  ~SpGEMM_variant_csr_dsparallel();

private:
  struct grs_variant this_variant;
};

}
#endif
