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

#include "SpGEMM_variant_bhSPARSE_cuda.h"

#include <ctime>

#include <bhsparse.h>

namespace EXCESS_GRS {

static void   perform_op(int argcin, int argcout,
                         grs_data** argvin, grs_data** argvout);
static double predict_cost(int argcin, int* in_state, int* in_size);
static double predict_switch(int argcin, int* in_state, int* in_size);

// Module global as the C style callback functions cannot access the
// instance members. Could perhaps be a class member instead?
static bool     cudaInitialized = false;
static bhsparse spgemm;

SpGEMM_variant_csr_bhSPARSE_cuda::
  SpGEMM_variant_csr_bhSPARSE_cuda(struct grs_component* component, int variant)
{
  grs_component_add_variant(component, &this_variant);
  grs_variant_init(&this_variant, perform_op, variant,
                   predict_cost, predict_switch);
  if (!cudaInitialized) {
    bool platforms[NUM_PLATFORMS];
    platforms[BHSPARSE_CUDA] = true;
    if (spgemm.initPlatform(platforms) != BHSPARSE_SUCCESS) {
      std::cout << ("SpGEMM_variant_bhSPARSE_cuda::"
                    "SpGEMM_variant_csr_bhSPARSE_cuda: Error: "
                    "Failed to initialize Cuda platform.")
                << std::endl;
    }
    cudaInitialized = true;
  }
}

SpGEMM_variant_csr_bhSPARSE_cuda::~SpGEMM_variant_csr_bhSPARSE_cuda()
{
  if (cudaInitialized) {
    spgemm.freePlatform();
    cudaInitialized = false;
  }
  grs_variant_destroy(&this_variant);
}

static void perform_op(int argcin, int argcout,
                       grs_data** argvin, grs_data** argvout)
{
  std::cout << "SpGEMM_variant_bhSPARSE_cuda::perform_op started." << std::endl;
  if (argcin != 8 || argcout != 4) {
    std::cout << ("SpGEMM_variant_bhSPARSE_cuda::perform_op: Error: "
                  "Wrong number of arguments.")
              << std::endl;
    return;
  }

#ifdef MODIFIED_BHSPARSE
  // NOTE: This requires a modified version of bhSPARSE.
  int A_m, A_n, A_nzmax; int* A_rp; int* A_ci; double* A_v;
  int B_m, B_n, B_nzmax; int* B_rp; int* B_ci; double* B_v;
  matrix_csr::convert_from_grs_input_for_gpu(argvin,
                                             A_m, A_n, A_nzmax,
                                             A_rp, A_ci, A_v);
  matrix_csr::convert_from_grs_input_for_gpu(argvin + 4,
                                             B_m, B_n, B_nzmax,
                                             B_rp, B_ci, B_v);
#else
  // FIXME: The matrices start on the CPU.
  SpMatrix A(0,0,0), B(0,0,0);
  matrix_csr::convert_from_grs_input(argvin,     A);
  matrix_csr::convert_from_grs_input(argvin + 4, B);
  int A_m = A.m; int A_n = A.n; int A_nzmax = A.nzmax;
  int B_m = B.m; int B_n = B.n; int B_nzmax = B.nzmax;
  int* Crp = (int*)calloc((A.m+1), sizeof(int));
#endif
  {
    struct timespec t1, t2; 
    std::cout << "Attempting matrix matrix multiplication "
              << (A_m) << "x" << (A_n) << " * "
              << (B_m) << "x" << (B_n) << " ... ";

    clock_gettime(CLOCK_REALTIME, &t1);

#ifdef MODIFIED_BHSPARSE
    if (spgemm.setDeviceData(A_m, A_n, B_n,
                             A_nzmax, A_v, A_rp, A_ci,
                             B_nzmax, B_v, B_rp, B_ci) != BHSPARSE_SUCCESS) {
#else
    if (spgemm.initData(A.m, A.n, B.n,
                        A.nzmax, A.v, A.rp, A.ci,
                        B.nzmax, B.v, B.rp, B.ci,
                        Crp) != BHSPARSE_SUCCESS) {
#endif
      std::cout << ("SpGEMM_variant_bhSPARSE_cuda::perform_op: Error: "
                    "Failed to setup bhSPARSE SpGEMM instance.")
                << std::endl;
    }

    if (spgemm.spgemm() != BHSPARSE_SUCCESS) {
      std::cout << ("SpGEMM_variant_bhSPARSE_cuda::perform_op: Error: "
                    "bhSPARSE SpGEMM spgemm() failed.")
                << std::endl;
    }

#ifdef MODIFIED_BHSPARSE
    // Read the GPU-side rp, ci and v arrays
    int C_nnz = spgemm.get_nnzC();
    int* C_rp; int* C_ci; double* C_v;
    spgemm.getDevice_C(C_rp, C_ci, C_v);
#else
    // Read C back to CPU.
    // FIXME: Should be delayed until necessary.
    int C_nnz  = spgemm.get_nnzC();
    SpMatrix C = SpMatrix(A.m, B.n, C_nnz);

    // Transfer the Crp array to the SpMatrix.
    std::free(C.rp);
    C.rp = Crp;

    // Read the GPU-side ci and v arrays into the SpMatrix.
    if (spgemm.get_C(C.ci, C.v) != BHSPARSE_SUCCESS) {
      std::cout << ("SpGEMM_variant_bhSPARSE_cuda::perform_op: Error: "
                    "bhSPARSE SpGEMM get_C() failed.")
                << std::endl;
    } 
#endif

    clock_gettime(CLOCK_REALTIME, &t2);

    std::cout << "Ok." << std::endl;
    std::cout << "% Result C is " << (A_m) << "x" << (B_n) << " matrix with "
              << (C_nnz) << " non-zeros." << std::endl;
    std::cout << "% Duration: "
              << ((double)(t2.tv_sec - t1.tv_sec) +
                  1e-9 * (double)(t2.tv_nsec - t1.tv_nsec))
              << " sec" << std::endl;

    // Reallocate the GRS output matrix.
#ifdef MODIFIED_BHSPARSE
    matrix_csr::convert_to_grs_gpu_output(A_m, B_n, C_nnz, C_rp, C_ci, C_v,
                                          argvout);
#else
    matrix_csr::convert_to_grs_output(C, argvout);
    matrix_csr::clear(A);
    matrix_csr::clear(B);
#endif
    /*
    if (spgemm.free_mem() != BHSPARSE_SUCCESS) {
      std::cout << ("SpGEMM_variant_bhSPARSE_cuda::perform_op: Error: "
                    "bhSPARSE SpGEMM free_mem() failed.")
                << std::endl;
    }
    */
  }
  std::cout << "SpGEMM_variant_bhSPARSE_cuda::perform_op finished."
            << std::endl;
}

static double predict_cost(int argcin, int* in_state, int* in_size)
{
  // FIXME.
  return 4.0;
}

static double predict_switch(int argcin, int* in_state, int* in_size)
{
  // FIXME.
  return 0.0;
}

}
