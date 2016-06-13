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

#ifndef __SPMATRIX_H
#define __SPMATRIX_H

#include <string>
#include <vector>

// Sparse matrix in CSR format.
class SpMatrix
{
public: // For now.
  int     nzmax;  // max #non-zeros
  int     m;      // #rows
  int     n;      // #columns
  int*    rp;     // row pointer array, indices 0 to m
  int*    ci;     // column index array, indices 0 to nz-1
  double* v;      // values, indices 0 to nz-1

public:
  typedef std::pair< int,int > RowColumnPair_t;
  typedef std::pair< RowColumnPair_t, double > MatrixTriple_t;
  struct MatrixRow_t {
    int     row;
    int     nzmax;
    int*    ci;
    double* v;
    
    MatrixRow_t(int row, int nzmax);
    ~MatrixRow_t();
  };

  SpMatrix(int rows, int columns);
  SpMatrix(int rows, int columns, int nzmax);
  SpMatrix(const SpMatrix& src);
  SpMatrix(int rows, int columns, std::vector<MatrixTriple_t> Mij);
  // NOTE: Assumes the entries are provided in row-major order.

  ~SpMatrix();

  SpMatrix& operator= (const SpMatrix& other);

  void SaveToFile(std::string filename);
  static SpMatrix LoadFromFile(std::string filename);

private:
  void Init(int rows, int columns, int nzmax);
};

bool RowMajorLessThan(SpMatrix::MatrixTriple_t& a,
                      SpMatrix::MatrixTriple_t& b);

#endif
