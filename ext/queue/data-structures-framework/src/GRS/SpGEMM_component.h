// GRS sparse CSR matrix-matrix multiplication component.
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
#ifndef __SPGEMM_GRS_COMP
#define __SPGEMM_GRS_COMP

#include <vector>
#include <iostream>

#include <grs.h>

#include <cstring>

#include <SpMatrix.h>

namespace EXCESS_GRS {

struct matrix_csr
{
  grs_data d_m_n_nzmax, d_rp, d_ci, d_v;
  struct grs_vector* vec_m_n_nzmax; // int vector of length 3.
  struct grs_vector* vec_rp;
  struct grs_vector* vec_ci;
  struct grs_vector* vec_v;

  matrix_csr(int m, int n);
  matrix_csr(SpMatrix matrix);

  ~matrix_csr();

  int rows() const;
  int columns() const;
  int nonzeros() const;

  void save_to_file(std::string filename);

  // Creates an independent SpMatrix instance.
  SpMatrix convert_to_SpMatrix();

  // Creates a SpMatrix instance that shares its storage with GRS.
  // NOTE: Beware of the SpMatrix destructor! Run clear() on the matrix
  //       before it goes out of scope.
  static void convert_from_grs_input(grs_data** argvin, SpMatrix& matrix);

  // To make a SpMatrix sharing storage with GRS safe for destruction.
  static void clear(SpMatrix& matrix);

  // Export the GPU side data buffers of one matrix_csr.
  static void convert_from_grs_input_for_gpu(grs_data** argvin,
                                             int& m, int& n, int& nzmax, 
                                             int*& gpu_rp,
                                             int*& gpu_ci,
                                             double*& gpu_v);

  static void convert_to_grs_output(SpMatrix& matrix, grs_data** argvout);

  static void convert_to_grs_gpu_output(int m, int n, int nzmax,
                                        int* gpu_rp, int* gpu_ci,
                                        double* gpu_v,
                                        grs_data** argvout);

private:
  void initialize(int m, int n, int nzmax);
  static float* read_grs_vector_on_cpu(grs_vector* vec_x);
  static float* read_grs_vector_on_gpu(grs_vector* vec_x);
  static void replace_grs_vector_on_cpu(grs_vector* vec,
                                        float* data,
                                        size_t size);
  static void replace_grs_vector_on_gpu(grs_vector* vec,
                                        float* data,
                                        size_t size);
};

class SpGEMM_component_csr
{
public:
  class variant
  {
  public:
    variant() {};
    virtual ~variant() {};
  private:
    // Disallow copying and assignment.
    variant(const variant&);
    void operator=(const variant&);
  };

  SpGEMM_component_csr();
  ~SpGEMM_component_csr();

  void setup_call(struct grs_call* call,
                  std::string      name,
                  matrix_csr&      A,
                  matrix_csr&      B,
                  matrix_csr&      Result);

private:
  // Disallow copying and assignment.
  SpGEMM_component_csr(const SpGEMM_component_csr&);
  void operator=(const SpGEMM_component_csr&);

  struct grs_component      this_component;
  std::vector< variant* >   variants;
};

}
#endif
