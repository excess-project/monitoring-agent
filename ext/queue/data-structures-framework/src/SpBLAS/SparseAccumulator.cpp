// Sparse accumulator for double matrix GEMM implementations
// Copyright (C) 2015 - 2016  Anders Gidenstam
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


#include "SparseAccumulator.h"

#include <cassert>
#include <cstdlib>
#include <iostream>

const double SparseAccumulator::zero = 0.0;

SparseAccumulator::SparseAccumulator(int n)
  : n(n), elems(0), version(1)
{
  assert(0 <= n);
  v    = (double*)std::malloc(n * sizeof(double));
  used = (int*)std::calloc(n, sizeof(int));
}

SparseAccumulator::~SparseAccumulator()
{
  std::free(used);
  std::free(v);
}

void
SparseAccumulator::Clear()
{
  elems = 0;
  nzs.clear();
  version++;
}

void
SparseAccumulator::AddTo(int index, double value)
{
  assert((0 <= index) && (index < n));
  if (used[index] != version) {
    elems++;
    used[index] = version;
    v[index]    = 0.0;
    nzs.push_back(index);
  }
  v[index] += value;
}

const double&
SparseAccumulator::operator[](int index)
{
  assert((0 <= index) && (index < n));
  if (used[index] == version) {
    return v[index];
  } else {
    return zero;
  }
}
