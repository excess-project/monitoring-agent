// Sparse double matrix for sparse BLAS type of operations.
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

#include "SpMatrix.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

SpMatrix::SpMatrix(int rows, int columns)
{
  Init(rows, columns, int(0.01*rows*columns));
}

SpMatrix::SpMatrix(int rows, int columns, int nzmax)
{
  Init(rows, columns, nzmax);
}

SpMatrix::SpMatrix(const SpMatrix& src)
{
  Init(src.m, src.n, src.nzmax);
  //std::cout << "SpMatrix::SpMatrix(): Copying " << m << "x" << n
  //          << " matrix with up to " << nzmax << " non-zeros." << std::endl;
  std::memcpy(rp, src.rp, (src.m+1)*sizeof(int));
  std::memcpy(ci, src.ci, src.nzmax*sizeof(int));
  std::memcpy(v, src.v, src.nzmax*sizeof(double));
}

// NOTE: The entries of Mij MUST be 0-based and in row-major order without
//       duplicates.
SpMatrix::SpMatrix(int rows, int columns, std::vector<MatrixTriple_t> Mij)
{
  Init(rows, columns, Mij.size());
  std::vector<MatrixTriple_t>::const_iterator mij = Mij.begin();
  std::vector<MatrixTriple_t>::const_iterator mend  = Mij.end();
  int r   = 0;
  int nnz = 0;
  for (; mij < mend; ++mij) {
    int i = (*mij).first.first;
    int j = (*mij).first.second;
    // Fill in row pointers until row j.
    for (; r <= i; ++r) {
      rp[r] = nnz;
    }
    // Store the matrix triple.
    ci[nnz] = j;
    v[nnz]  = (*mij).second;
    nnz++;
  }
  // Fill in the rest of the row pointers.
  for (; r <= m; ++r) {
    rp[r] = nnz;
  }
  assert(nnz == nzmax);
}

SpMatrix::~SpMatrix()
{
  //std::cout << "SpMatrix::~SpMatrix(): Destroying " << m << "x" << n
  //          << " matrix with " << nzmax << " non-zeros ... ";
  std::free(v);
  std::free(ci);
  std::free(rp);
  //std::cout << "Done." << std::endl;
}

// Copy assignment operator
SpMatrix& SpMatrix::operator= (const SpMatrix& other)
{
  if (this != &other) {
    // Re-use copy-constructor
    SpMatrix tmp(other);
    // Swap content with tmp (which will then be destroyed).
    std::swap(this->m, tmp.m);
    std::swap(this->n, tmp.n);
    std::swap(this->nzmax, tmp.nzmax);
    std::swap(this->rp, tmp.rp);
    std::swap(this->ci, tmp.ci);
    std::swap(this->v, tmp.v);
  }
  return *this;
}

void
SpMatrix::SaveToFile(std::string filename)
{
  std::ofstream ofs;

  ofs.open(filename.c_str());

  ofs << "%%MatrixMarket matrix coordinate real general" << std::endl;
  ofs << m << " " << n << " " << nzmax << std::endl;

  for (int r = 0; r < m; r++) {
    for (int c = rp[r]; c < rp[r+1]; c++) {
      // NOTE: MatrixMarket triplets use 1-based indices.
      ofs << (r+1) << " " << (ci[c]+1) << " " << v[c] << std::endl;
    }
  }
  ofs.close();
}

SpMatrix
SpMatrix::LoadFromFile(std::string filename)
{
  std::ifstream ifs;
  int m, n, nnz;
  std::string line;

  ifs.open(filename.c_str(), std::ifstream::in);
  
  do {
    std::getline(ifs, line);
  } while (line[0] == '%');
  std::stringstream(line) >> m >> n >> nnz;
  
  std::vector<MatrixTriple_t> Mij;

  while (ifs.good()) {
    int i, j;
    double v;
    std::getline(ifs, line);
    std::stringstream(line) >> i >> j >> v;
    // NOTE: The MatrixMarket files use 1-based indices while SpMatrix
    //       uses 0-based.
    Mij.push_back(MatrixTriple_t(RowColumnPair_t(i-1,j-1), v));
  }
  ifs.close();

  // The order of the entries is not guaranteed!
  // Fix by sorting in row-major order.
  std::sort(Mij.begin(), Mij.end(), RowMajorLessThan);

  // Merge duplicated entries by summation (Octave default)
  // or keep last (Octave 'unique') or keep first (possibly librsb default).
  std::vector<MatrixTriple_t>::iterator mij = Mij.begin();
  ++mij;
  while (mij < Mij.end()) {
    if ((mij-1)->first == mij->first) {
      switch (1) {
      case 0:
        // Sum.
        (mij-1)->second += mij->second;
        break;
      case 1:
        // Keep last.
        (mij-1)->second = mij->second;
        break;
      case 2:
      default:
        // Keep first.
        break;
      }
      mij = Mij.erase(mij);
    } else {
      ++mij;
    }
  }

  return SpMatrix(m, n, Mij);
}

void
SpMatrix::Init(int rows, int columns, int nzmax)
{
  this->nzmax = nzmax;
  this->m = rows;
  this->n = columns;
  //std::cout << "SpMatrix::SpMatrix(): Created " << m << "x" << n
  //          << " matrix with up to " << nzmax << " non-zeros." << std::endl;
  rp = (int*)std::calloc(m+1, sizeof(int));
  ci = (int*)std::calloc(nzmax, sizeof(int));
  v  = (double*)std::calloc(nzmax, sizeof(double));
}

SpMatrix::MatrixRow_t::MatrixRow_t(int row, int nzmax)
{
  this->row   = row;
  this->nzmax = nzmax;
  ci = (int*)malloc(nzmax * sizeof(int));
  v  = (double*)malloc(nzmax * sizeof(double));
}

SpMatrix::MatrixRow_t::~MatrixRow_t()
{
  std::free(v);
  std::free(ci);
}

bool RowMajorLessThan(SpMatrix::MatrixTriple_t& a,
                      SpMatrix::MatrixTriple_t& b)
{
  return
    (a.first.first < b.first.first) ||
    (a.first.first == b.first.first && a.first.second < b.first.second);
}
