// Concurrent data structure based parallel sparse double matrix GEMM
// implementation.
// New(?) SpMM_DSParallel_RowStore/TripletStore algorithms by Anders Gidenstam,
// inspired by the classical sequential algorithms in [Gustavson, ACM TMS 4(3),
// 1978] and [Gilbert et al., SIAM JMAA 13(1), 1992].
//
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
#ifndef __SPGEMM_CDS_H
#define __SPGEMM_CDS_H

#include <omp.h>

#include <algorithm>
#include <vector>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>

#include "primitives.h"
#include "SpMatrix.h"
#include "SparseAccumulator.h"

#define WU 1

// Internal function: 1st parallel phase.
template < template < typename T > class concurrent_bag_t >
void SpGEMM_DSParallel_RowStore_1P
  (double alpha, const SpMatrix& A, double beta, const SpMatrix& B,
   concurrent_bag_t< SpMatrix::MatrixRow_t >* Ci_bag,
   volatile int* nextci,
   int* element_count)
{
  typename concurrent_bag_t< SpMatrix::MatrixRow_t >::handle* Ci =
    Ci_bag->get_handle();
  double alphabeta = alpha*beta;
  SparseAccumulator SPA(B.n);
  int ci;

  while (1) {

    ci = FAA32(nextci, WU);
    int ciend = std::min(ci+WU, A.m);

    if (!(ci < A.m)) {
      break;
    }

    for (; ci < ciend; ++ci) {
      int aij  = A.rp[ci];
      int aend = A.rp[ci + 1];
      for (; aij < aend; ++aij) {
        int j    = A.ci[aij];
        int bjk  = B.rp[j];
        int bend = B.rp[j+1];
        double vaij = A.v[aij];
        for (; bjk < bend; ++bjk) {
          int k = B.ci[bjk];
          SPA.AddTo(k, vaij*B.v[bjk]);
        }
      }

      // Save Ci* that is stored in the SPA.
      std::vector<int>::const_iterator cik  = SPA.nzs.begin();
      std::vector<int>::const_iterator cend = SPA.nzs.end();
      int nnz = 0;
      SpMatrix::MatrixRow_t* row = new SpMatrix::MatrixRow_t(ci, cend - cik);
      for (; cik < cend; ++cik) {
        double vcik = alphabeta*SPA.v[*cik];
        if (vcik != 0.0) { // FIXME: cut off very small values too?
          row->ci[nnz] = *cik;
          row->v[nnz]  = vcik;
          nnz++;
        }
      }
      // Save the true #nz.
      element_count[ci] = nnz;
      Ci->insert(row);
      SPA.Clear();
    }
  }
  delete Ci;
}

// Internal function: Sequential phase.
template < template < typename T > class concurrent_bag_t >
void SpGEMM_DSParallel_RowStore_2S
  (SpMatrix& C,
   int* element_count)
{
  // Prepare the row pointers.
  int nnz = 0;
  int ci;
  for (ci = 0; ci < C.m; ++ci) {
    C.rp[ci] = nnz;
    nnz += element_count[ci];
  }
  C.rp[ci] = nnz;

  // Reallocate the ci and v vectors in C.
  C.nzmax = nnz;
  std::free(C.ci);
  std::free(C.v);
  C.ci = (int*)std::malloc(C.nzmax * sizeof(int));
  C.v  = (double*)std::malloc(C.nzmax * sizeof(double));
}

// Internal function: 2nd parallel phase.
template < template < typename T > class concurrent_bag_t >
void SpGEMM_DSParallel_RowStore_3P
  (SpMatrix& C,
   concurrent_bag_t< SpMatrix::MatrixRow_t >* Ci_bag,
   int* element_count)
{
  typename concurrent_bag_t< SpMatrix::MatrixRow_t >::handle* Ci =
    Ci_bag->get_handle();
  SpMatrix::MatrixRow_t* ci;
  while (Ci->try_remove_any(ci)) {
    int kbegin = C.rp[ci->row];
    int knnz   = element_count[ci->row];
#ifdef _MEMCPY
    // It is not clear if memcpy is any faster. The bigger loop body below
    // might allow better optimization.
    std::memcpy(&C.ci[kbegin], ci->ci, knnz * sizeof(int));
    std::memcpy(&C.v[kbegin], ci->v, knnz * sizeof(double));
#else
    for (int k = 0; k < knnz; ++k) {
      C.ci[kbegin + k] = ci->ci[k];
      C.v[kbegin + k]  = ci->v[k];
    }
#endif
    delete ci;
  }
  delete Ci;
}

template < template < typename T > class concurrent_bag_t >
SpMatrix SpGEMM_DSParallel_RowStore(double alpha, const SpMatrix& A,
                                    double beta,  const SpMatrix& B)
{
  assert(A.n == B.m);
  volatile int nextci = 0;
  concurrent_bag_t< SpMatrix::MatrixRow_t >* Ci_bag =
    new concurrent_bag_t< SpMatrix::MatrixRow_t >();
  int* element_count = (int*)calloc(A.m, sizeof(int));
  SpMatrix C(0, 0, 0);
  volatile int phase1 = 0;
  volatile int phase2 = 0;
  volatile int phase3 = 0;
  struct timespec sleep_time;
  sleep_time.tv_sec  = 0;
  sleep_time.tv_nsec = 0;

#pragma omp parallel
  { 
    FAA32(&phase1, 1);
    SpGEMM_DSParallel_RowStore_1P<concurrent_bag_t>(alpha, A, beta, B,
                                                    Ci_bag,
                                                    &nextci, element_count);
    FAA32(&phase1, -1); // This should be safe as no worker exits phase 1
                        // before all work units have been taken (but not
                        // necessarily processed) so any late arrivals that
                        // hasn't increased phase1 yet won't touch anything.

    // Wait for phase 1 to end.
    while (phase1 > 0) {
      nanosleep(&sleep_time, NULL);
    }

    // Allow exactly one worker to perfrom the sequential phase.
    {
      int p2 = phase2;
      if (!p2) {
        FAA32(&phase2, 1);
      }
      if (!p2) {
        // Perform the sequential phase 2. Done by the first thread through.
        // Create the result matrix C.
        C = SpMatrix(A.m, B.n, 0);

        // Sequential preparation of C.
        SpGEMM_DSParallel_RowStore_2S<concurrent_bag_t>(C, element_count);

        // Launch phase 3.
        FAA32(&phase3, 1);
      } else {
        // Wait for phase 3 to begin.
        while (phase3 == 0) {
          nanosleep(&sleep_time, NULL);
        }
      }
    }
    
    // Phase 3: Fill C in parallel from the row store.
    SpGEMM_DSParallel_RowStore_3P<concurrent_bag_t>(C, Ci_bag, element_count);
  }
  std::free(element_count);
  delete Ci_bag;
  return C;
}

template < template < typename T > class concurrent_bag_t >
SpMatrix SpMM_DSParallel_RowStore(const SpMatrix& A, const SpMatrix& B)
{
  return SpGEMM_DSParallel_RowStore<concurrent_bag_t>(1.0, A, 1.0, B);
}

template < template < typename T > class concurrent_bag_t >
SpMatrix SpGEMM_DSParallel_TripletStore(double alpha, const SpMatrix& A,
                                        double beta,  const SpMatrix& B)
{
  assert(A.n == B.m);
  double alphabeta = alpha*beta;
  volatile int nextci = 0;
  concurrent_bag_t< SpMatrix::MatrixTriple_t >* Cik_bag =
    new concurrent_bag_t< SpMatrix::MatrixTriple_t >();
  int* element_count = (int*)calloc(A.m, sizeof(int));

#pragma omp parallel
  {
    typename concurrent_bag_t< SpMatrix::MatrixTriple_t >::handle* Cik =
      Cik_bag->get_handle();
    SparseAccumulator SPA(B.n);
    int ci;

    while (1) {

      ci = FAA32(&nextci, WU);
      int ciend = std::min(ci+WU, A.m);

      if (!(ci < A.m)) {
        break;
      }

      for (; ci < ciend; ++ci) {
        int aij  = A.rp[ci];
        int aend = A.rp[ci + 1];
        for (; aij < aend; ++aij) {
          int j    = A.ci[aij];
          int bjk  = B.rp[j];
          int bend = B.rp[j+1];
          double vaij = A.v[aij];
          for (; bjk < bend; ++bjk) {
            int k = B.ci[bjk];
            SPA.AddTo(k, vaij*B.v[bjk]);
          }
        }
        
        // Save Ci* that is stored in the SPA.
        std::vector<int>::const_iterator cik  = SPA.nzs.begin();
        std::vector<int>::const_iterator cend = SPA.nzs.end();
        int nnz = 0;
        for (; cik < cend; ++cik) {
          double vcik = alphabeta*SPA.v[*cik];
          if (vcik != 0.0) { // FIXME: cut off very small values too?
            nnz++;
            Cik->insert
              (new SpMatrix::MatrixTriple_t(SpMatrix::RowColumnPair_t(ci,*cik),
                                            vcik));
          }
        }
        element_count[ci] = nnz;
        SPA.Clear();
      }
    }
    delete Cik;
  } // end parallel

  // Create the result matrix C.
  SpMatrix C(A.m, B.n, 0);

  // Prepare the row pointers.
  int nnz = 0;
  int ci;
  for (ci = 0; ci < C.m; ++ci) {
    nnz += element_count[ci];
    C.rp[ci] = nnz;
  }
  C.rp[ci] = nnz;

  // Reallocate the ci and v vectors in C.
  C.nzmax = nnz;
  std::free(C.ci);
  std::free(C.v);
  C.ci = (int*)std::malloc(C.nzmax * sizeof(int));
  C.v  = (double*)std::malloc(C.nzmax * sizeof(double));

  // Fill C in parallel from the triplet store.
#pragma omp parallel
  {
    typename concurrent_bag_t< SpMatrix::MatrixTriple_t >::handle* Cik =
      Cik_bag->get_handle();
    SpMatrix::MatrixTriple_t* cik;
    while (Cik->try_remove_any(cik)) {
      int i    = cik->first.first;
      int k    = cik->first.second;
      double v = cik->second;
      delete cik;
      int idx = FAA32(&C.rp[i],-1) - 1;
      if (idx < 0) {
        std::cout << "Bad idx for triple (" << i << ", " << k << ", "
                  << v << ")" << std::endl;
      }
      C.ci[idx] = k;
      C.v[idx]  = v;
    }
    delete Cik;
  } // end parallel

  std::free(element_count);
  //std::cout << "SpMM(): Got " << Cik.size() << " non-zeros.";
  return C;
}

template < template < typename T > class concurrent_bag_t >
SpMatrix SpMM_DSParallel_TripletStore(const SpMatrix& A, const SpMatrix& B)
{
  return SpGEMM_DSParallel_TripletStore<concurrent_bag_t>(1.0, A, 1.0, B);
}

#endif
