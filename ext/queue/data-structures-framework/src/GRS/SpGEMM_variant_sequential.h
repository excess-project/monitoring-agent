// GRS sequential sparse CSR matrix-matrix multiplication variant.
// The implementation based on the classical algorithms in
// [Gustavson, ACM TMS 4(3), 1978] and [Gilbert et al., SIAM JMAA 13(1), 1992]
// with some modifications.
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
#ifndef __SPGEMM_GRS_VAR_SEQUENTIAL
#define __SPGEMM_GRS_VAR_SEQUENTIAL

#include "SpGEMM_component.h"

namespace EXCESS_GRS {

class SpGEMM_variant_csr_sequential : public SpGEMM_component_csr::variant
{
public:
  SpGEMM_variant_csr_sequential(struct grs_component* component, int variant);
  ~SpGEMM_variant_csr_sequential();

private:
  struct grs_variant this_variant;
};

}
#endif
