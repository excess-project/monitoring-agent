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

#include "SpGEMM_component.h"

#include "SpGEMM_variant_sequential.h"
#include "SpGEMM_variant_DSParallel.h"
#include "SpGEMM_variant_bhSPARSE_cuda.h"

namespace EXCESS_GRS {

SpGEMM_component_csr::SpGEMM_component_csr()
{
  grs_component_init(&this_component);
  grs_component_set_name(&this_component, "SpGEMM_component_csr");
  // Add all available variants here?
  // NOTE: Currently exactly 2 variants are supported where variant 1 is
  //       assumed to be GPU based.
  //variants.push_back(new SpGEMM_variant_csr_sequential(&this_component, 0));
  variants.push_back(new SpGEMM_variant_csr_dsparallel(&this_component, 0));
  variants.push_back(new SpGEMM_variant_csr_bhSPARSE_cuda(&this_component, 1));
}

SpGEMM_component_csr::~SpGEMM_component_csr()
{
  grs_component_destroy(&this_component);
}

void
SpGEMM_component_csr::setup_call(struct grs_call* call,
                                 std::string      name,
                                 matrix_csr&      A,
                                 matrix_csr&      B,
                                 matrix_csr&      Result)
{
  grs_call_init(call, &this_component);
  grs_call_set_name(call, (char*)name.c_str());
  // Setup the call arguments.
  // Input matrix A
  grs_call_add_input_opnd(call, &A.d_m_n_nzmax);
  grs_call_add_input_opnd(call, &A.d_rp);
  grs_call_add_input_opnd(call, &A.d_ci);
  grs_call_add_input_opnd(call, &A.d_v);
  // Input matrix B
  grs_call_add_input_opnd(call, &B.d_m_n_nzmax);
  grs_call_add_input_opnd(call, &B.d_rp);
  grs_call_add_input_opnd(call, &B.d_ci);
  grs_call_add_input_opnd(call, &B.d_v);
  // Output matrix C
  grs_call_add_output_opnd(call, &Result.d_m_n_nzmax);
  grs_call_add_output_opnd(call, &Result.d_rp);
  grs_call_add_output_opnd(call, &Result.d_ci);
  grs_call_add_output_opnd(call, &Result.d_v);
}

//-----------------------------------------------------------------------------
// matrix_csr below

matrix_csr::matrix_csr(int m, int n)
{
  initialize(m, n, 0);
}

matrix_csr::matrix_csr(SpMatrix matrix)
{
  initialize(matrix.m, matrix.n, matrix.nzmax);
  // Copy the matrix contents. 
  std::memcpy(read_grs_vector_on_cpu(vec_rp),
              matrix.rp,
              (matrix.m+1) * sizeof(int));
  std::memcpy(read_grs_vector_on_cpu(vec_ci),
              matrix.ci,
              matrix.nzmax * sizeof(int));
  std::memcpy(read_grs_vector_on_cpu(vec_v),
              matrix.v,
              matrix.nzmax * sizeof(double));
}

matrix_csr::~matrix_csr()
{
  std::cout << "SpGEMM_component.cpp::~matrix_csr()" << std::endl;
  // NOTE: The actual data blocks should also be freed.
  free(vec_m_n_nzmax);
  free(vec_rp);
  free(vec_ci);
  free(vec_v);
}

int matrix_csr::rows() const
{
  int* m_n_nzmax = (int*)read_grs_vector_on_cpu(vec_m_n_nzmax);
  return m_n_nzmax[0];
}

int matrix_csr::columns() const
{
  int* m_n_nzmax = (int*)read_grs_vector_on_cpu(vec_m_n_nzmax);
  return m_n_nzmax[1];
}

int matrix_csr::nonzeros() const
{
  int* m_n_nzmax = (int*)read_grs_vector_on_cpu(vec_m_n_nzmax);
  return m_n_nzmax[2];
}

void matrix_csr::save_to_file(std::string filename)
{
  SpMatrix matrix = convert_to_SpMatrix();
  matrix.SaveToFile(filename);
}

// Creates an independent SpMatrix instance.
SpMatrix matrix_csr::convert_to_SpMatrix()
{
  int* m_n_nzmax = (int*)read_grs_vector_on_cpu(vec_m_n_nzmax);
  SpMatrix matrix = SpMatrix(m_n_nzmax[0],
                             m_n_nzmax[1],
                             m_n_nzmax[2]);
  std::memcpy(matrix.rp,
              read_grs_vector_on_cpu(vec_rp),
              (matrix.m+1) * sizeof(int));
  std::memcpy(matrix.ci,
              read_grs_vector_on_cpu(vec_ci),
              matrix.nzmax * sizeof(int));
  std::memcpy(matrix.v,
              read_grs_vector_on_cpu(vec_v),
              matrix.nzmax * sizeof(double));
  return matrix;
}

// Creates a SpMatrix instance that shares its storage with GRS.
// NOTE: Beware of the SpMatrix destructor! Run clear() on the matrix
//       before it goes out of scope.
void matrix_csr::convert_from_grs_input(grs_data** argvin,
                                        SpMatrix& matrix)
{
  // Remove the old matrix content.
  std::free(matrix.rp);
  std::free(matrix.ci);
  std::free(matrix.v);

  grs_vector* vec_m_n_nzmax = (grs_vector*)argvin[0]->d; // matrix m; n; nzmax
  grs_vector* vec_rp = (grs_vector*)argvin[1]->d; // matrix rp
  grs_vector* vec_ci = (grs_vector*)argvin[2]->d; // matrix ci
  grs_vector* vec_v  = (grs_vector*)argvin[3]->d; // matrix v

  // Read m_n_nzmax.
  int* m_n_nzmax = (int*)read_grs_vector_on_cpu(vec_m_n_nzmax);

  // Initialize the SpMatrix using the already existing storage.
  matrix.m = m_n_nzmax[0];
  matrix.n = m_n_nzmax[1];
  matrix.nzmax = m_n_nzmax[2];
  matrix.rp = (int*)read_grs_vector_on_cpu(vec_rp);
  matrix.ci = (int*)read_grs_vector_on_cpu(vec_ci);
  matrix.v  = (double*)read_grs_vector_on_cpu(vec_v);
}

// To make a SpMatrix sharing storage with GRS safe for destruction.
void matrix_csr::clear(SpMatrix& matrix)
{
  matrix.nzmax = 0;
  matrix.rp = 0;
  matrix.ci = 0;
  matrix.v  = 0;
}

// Export the GPU side data buffers of one matrix_csr.
void matrix_csr::convert_from_grs_input_for_gpu(grs_data** argvin,
                                                int& m, int& n, int& nzmax, 
                                                int*& gpu_rp,
                                                int*& gpu_ci,
                                                double*& gpu_v)
{
  grs_vector* vec_m_n_nzmax = (grs_vector*)argvin[0]->d; // matrix m; n; nzmax
  grs_vector* vec_rp = (grs_vector*)argvin[1]->d; // matrix rp
  grs_vector* vec_ci = (grs_vector*)argvin[2]->d; // matrix ci
  grs_vector* vec_v  = (grs_vector*)argvin[3]->d; // matrix v

  // Read m_n_nzmax.
  int* m_n_nzmax = (int*)read_grs_vector_on_cpu(vec_m_n_nzmax);

  m     = m_n_nzmax[0];
  n     = m_n_nzmax[1];
  nzmax = m_n_nzmax[2];
  gpu_rp = (int*)read_grs_vector_on_gpu(vec_rp);
  gpu_ci = (int*)read_grs_vector_on_gpu(vec_ci);
  gpu_v  = (double*)read_grs_vector_on_gpu(vec_v);
}

void matrix_csr::convert_to_grs_output(SpMatrix& matrix, grs_data** argvout)
{
  // NOTE: The matrix argument will be destroyed. 

  // argvout mapping:
  //   argvout[0]:  m_n_nzmax
  //   argvout[1]:  rp
  //   argvout[2]:  ci
  //   argvout[3]:  v

  // Set m_n_nzmax using the existing result matrix.
  ((grs_vector*)argvout[0]->d)->mode = 0;
  int* m_n_nzmax = (int*)((grs_vector*)argvout[0]->d)->d_cpu;
  m_n_nzmax[0] = matrix.m;
  m_n_nzmax[1] = matrix.n;
  m_n_nzmax[2] = matrix.nzmax;

  // Set rp using the existing result matrix.
  replace_grs_vector_on_cpu((grs_vector*)argvout[1]->d,
                            (float*)matrix.rp,
                            (matrix.m*sizeof(int))/sizeof(float));

  // Set ci using the existing result matrix.
  replace_grs_vector_on_cpu((grs_vector*)argvout[2]->d,
                            (float*)matrix.ci,
                            (matrix.nzmax*sizeof(int))/sizeof(float));

  // Set v using the existing result matrix.
  replace_grs_vector_on_cpu((grs_vector*)argvout[3]->d,
                            (float*)matrix.v,
                            (matrix.nzmax*sizeof(double))/sizeof(float));

  clear(matrix);
}

void matrix_csr::convert_to_grs_gpu_output(int m, int n, int nzmax,
                                           int* gpu_rp, int* gpu_ci,
                                           double* gpu_v,
                                           grs_data** argvout)
{
  // NOTE: The GPU side buffers now belong to GRS.

  // argvout mapping:
  //   argvout[0]:  m_n_nzmax
  //   argvout[1]:  rp
  //   argvout[2]:  ci
  //   argvout[3]:  v

  // Set m_n_nzmax.
  ((grs_vector*)argvout[0]->d)->mode = 0;
  int* m_n_nzmax = (int*)((grs_vector*)argvout[0]->d)->d_cpu;
  m_n_nzmax[0] = m;
  m_n_nzmax[1] = n;
  m_n_nzmax[2] = nzmax;

  // Set rp, ci and v using the existing result.
  replace_grs_vector_on_gpu((grs_vector*)argvout[1]->d,
                            (float*)gpu_rp,
                            (m*sizeof(int))/sizeof(float));
  replace_grs_vector_on_gpu((grs_vector*)argvout[2]->d,
                            (float*)gpu_ci,
                            (nzmax*sizeof(int))/sizeof(float));
  replace_grs_vector_on_gpu((grs_vector*)argvout[3]->d,
                            (float*)gpu_v,
                            (nzmax*sizeof(double))/sizeof(float));
}

void matrix_csr::initialize(int m, int n, int nzmax)
{
  int* m_n_nzmax = (int*)malloc(3*sizeof(int));
  m_n_nzmax[0] = m;
  m_n_nzmax[1] = n;
  m_n_nzmax[2] = nzmax;

  grs_data_init(&d_m_n_nzmax, GRS_VECTOR);
  grs_data_init(&d_rp, GRS_VECTOR);
  grs_data_init(&d_ci, GRS_VECTOR);
  grs_data_init(&d_v,  GRS_VECTOR);

  vec_m_n_nzmax = (struct grs_vector*)malloc(sizeof(struct grs_vector));
  vec_rp        = (struct grs_vector*)malloc(sizeof(struct grs_vector));
  vec_ci        = (struct grs_vector*)malloc(sizeof(struct grs_vector));
  vec_v         = (struct grs_vector*)malloc(sizeof(struct grs_vector));
  
  grs_vector_init(vec_m_n_nzmax, (float*)m_n_nzmax, 4, "m_n_nzmax");
  grs_vector_init(vec_rp, (float*)malloc((m+1)*sizeof(int)),
                  (m*sizeof(int))/sizeof(float), "rp");
  grs_vector_init(vec_ci, (float*)malloc(nzmax*sizeof(int)),
                  (nzmax*sizeof(int))/sizeof(float), "ci");
  grs_vector_init(vec_v,  (float*)malloc(nzmax*sizeof(double)),
                  (nzmax*sizeof(double))/sizeof(float), "v");

  grs_data_set_vector(&d_m_n_nzmax, vec_m_n_nzmax);
  grs_data_set_vector(&d_rp, vec_rp);
  grs_data_set_vector(&d_ci, vec_ci);
  grs_data_set_vector(&d_v,  vec_v);
}

float* matrix_csr::read_grs_vector_on_cpu(grs_vector* vec_x)
{
  float* x = (float*)vec_x->d_cpu;
  int size = vec_x->size;

  if(vec_x->mode==1) {
    std::cout << "read_grs_vector_on_cpu: Refresh from GPU." << std::endl;
    copy_from_gpu((float*)vec_x->d_cpu, (float*)vec_x->d_gpu, size);
    vec_x->mode = 0;
  }
  return x;
}

float* matrix_csr::read_grs_vector_on_gpu(grs_vector* vec_x)
{
  float* x = (float*)vec_x->d_gpu;
  int size = vec_x->size;

  if(vec_x->mode==0) {
    std::cout << "read_grs_vector_on_gpu: Refresh from CPU." << std::endl;
    copy_to_gpu((float*)vec_x->d_gpu, (float*)vec_x->d_cpu, size);
    vec_x->mode = 1;
  }
  return x;
}

void matrix_csr::replace_grs_vector_on_cpu(grs_vector* vec,
                                           float* data,
                                           size_t size)
{
  vec->mode = 0;
  std::free(vec->d_cpu);
  vec->d_cpu = data;
  if (vec->size != size) {
    // GPU side storage must be resized.
    vec->size = size;
    free_data_on_gpu(vec->d_gpu);
    vec->d_gpu = allocate_on_gpu(&vec->d_gpu, size);
  }
}

void matrix_csr::replace_grs_vector_on_gpu(grs_vector* vec,
                                           float* data,
                                           size_t size)
{
  vec->mode = 1;
  free_data_on_gpu(vec->d_gpu);
  vec->d_gpu = data;
  if (vec->size != size) {
    // GPU side storage must be resized.
    vec->size = size;
    std::free(vec->d_cpu);
    vec->d_cpu = malloc(size * sizeof(float));
  }
}

}
