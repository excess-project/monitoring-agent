// Sparse accumulator for double matrix GEMM implementations
// Copyright (C) 2015  Anders Gidenstam
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
#ifndef __SPARSE_ACCUMULATOR_H
#define __SPARSE_ACCUMULATOR_H

#include <vector>

#include <cassert>
#include <cstdlib>
#include <iostream>

class SparseAccumulator // a.k.a. SPA.
{
public: // For now.
  int              n;
  int              elems;
  int              version;
  double*          v;
  int*             used;
  std::vector<int> nzs;

  static const double zero;

public:
  SparseAccumulator(int n);
  ~SparseAccumulator();

  void Clear();
  void AddTo(int index, double value);

  const double& operator[](int index);

};

#endif
