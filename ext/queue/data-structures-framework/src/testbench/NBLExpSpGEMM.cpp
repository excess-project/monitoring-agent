// Sparse matrix-matrix multiplication benchmark for the EXCESS experiment
// framework.
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

#include "NBLExpSpGEMM.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctime>
#include <unistd.h>

#include "primitives.h"
#include "SparseAccumulator.h"
#include "SpGEMM_Gustavson.h"

static bool VerifySpMMSquare(const SpMatrix A, const SpMatrix C);

static const char* MATRIX_FILES_INIT[] = {
  "matrices/general/R14.mtx",
  "matrices/general/R15.mtx",
  "matrices/general/R16.mtx",
  "matrices/general/ASIC_320k.mtx",
  "matrices/general/cage12.mtx",
  "matrices/general/m133-b3.mtx",
  "matrices/general/majorbasis.mtx",
  "matrices/general/patents_main.mtx",
  "matrices/general/poisson3Da.mtx",
  "matrices/general/rajat31.mtx",
  "matrices/general/scircuit.mtx",
  "matrices/general/sme3Dc.mtx",
  "matrices/general/torso1.mtx",
  "matrices/general/webbase-1M.mtx"
};

static const std::vector<std::string>
  MATRIX_FILES(MATRIX_FILES_INIT,
               MATRIX_FILES_INIT + 
               sizeof(MATRIX_FILES_INIT)/sizeof(MATRIX_FILES_INIT[0]));


NBLExpSpGEMM::NBLExpSpGEMM(void)
  : WUSize(4), matrix(0), mmalg(1),
    A(SpMatrix(0, 0, 0)), C(SpMatrix(0, 0, 0)),
    phase1(0), phase2(0), phase3(0), nextci(0), element_count(0)
{
#ifdef SPGEMM_USE_MUTEX
  pthread_mutex_init(&phase2_mutex, NULL);
  pthread_cond_init(&phase2_condition, NULL);
#endif
#if SPGEMM_DEBUG
  row_P1_cycles = 0;
  row_P3_cycles = 0;
#endif
}

NBLExpSpGEMM::~NBLExpSpGEMM(void)
{
#ifdef SPGEMM_DEBUG
  // Print all per-row cycle and non-zeros counts.
  std::cout << "%% Per-row phase 1 and phase 3 cycle "
            << "and non-zero elements counts:" << std::endl;
  for (int r = 0; r < A.m; r++) {
    std::cout << r << "  "
              << row_P1_cycles[r] << "  "
              << row_P3_cycles[r] << "  "
              << element_count[r] << std::endl;
  }

  std::free(row_P3_cycles);
  std::free(row_P1_cycles);
#endif

  std::free(element_count);
#ifdef SPGEMM_USE_MUTEX
  pthread_mutex_destroy(&phase2_mutex);
  pthread_cond_destroy(&phase2_condition);
#endif

}

string NBLExpSpGEMM::GetExperimentName()
{
  return string("SpGEMM");
}

string NBLExpSpGEMM::GetCaseName()
{
  std::stringstream ss;
  ss << "-M" << matrix
     << "-MM" << mmalg
     << "-WU" << WUSize;
  return ss.str();
}

vector<string> NBLExpSpGEMM::GetParameters()
{
  vector<string> v;
  v.push_back(string("Matrix"));
  v.push_back(string("Algorithm"));
  v.push_back(string("Work unit size (rows)"));
  return v;
}


vector<string> NBLExpSpGEMM::GetParameterValues(int pno)
{
  vector<string> v;
  switch (pno) {
  case 0:
    {
      std::stringstream ss;

      for (int i = 0; i < MATRIX_FILES.size(); i++) {
        ss << "Matrix " << i << ". " << MATRIX_FILES[i] << "  ";
      }
      ss << "(default 0)";
      v.push_back(ss.str());
    }
    break;
  case 1:
    v.push_back(string("Matrix-matrix multiplication algorithm 0. sequential Gustavson. 1. new D-S parallel (default 1)"));
    break;
  case 2:
    v.push_back(string("Work unit size in rows of A (default 4)"));
    break;
  }
  return v;
}

int NBLExpSpGEMM::GetParameter(int pno)
{
  switch (pno) {
  case 0:
    return matrix;
  case 1:
    return mmalg;
  case 2:
    return WUSize;
  default:
    return -1;
  }
}

void NBLExpSpGEMM::SetParameter(int pno, int value)
{
  switch (pno) {
  case 0:
    matrix=value;
    break;
  case 1:
    mmalg=value;
  case 2:
    WUSize=value;
  default:
    break;
  }
}

void NBLExpSpGEMM::CreateScenario()
{
  int i;

  // Load the matrix A.
  A = SpMatrix::LoadFromFile(MATRIX_FILES[matrix]);

  // Create the result matrix C.
  C = SpMatrix(A.m, A.n, 0);
  element_count = (int*)calloc(A.m, sizeof(int));
#ifdef SPGEMM_DEBUG
  row_P1_cycles = (unsigned long*)calloc(A.m, sizeof(unsigned long));
  row_P3_cycles = (unsigned long*)calloc(A.m, sizeof(unsigned long));
#endif
  //std::cout << "Created the matrix A as " << (A.m) << "x" << (A.n)
  //          << " matrix with " << (A.nzmax) << " non-zeros." << std::endl;
  //std::cout << "Created the matrix C as " << (C.m) << "x" << (C.n)
  //          << " matrix with " << (C.nzmax) << " non-zeros." << std::endl;
}

void NBLExpSpGEMM::InitImplementationNr(int nr)
{
  NBLExpProducerConsumerBase::InitImplementationNr(nr);
  // Initialization for one m-m multiplication.
  phase1 = 0;
  phase2 = 0;
  phase2_start = 0;
  phase3 = 0;
  phase3_start = 0;
  nextci = 0;
  for (int i = 0; i < A.m; ++i) {
    element_count[i] = 0;
  }
  //std::cout << "Preparing to square a " << (A.m) << "x" << (A.n)
  //          << " matrix with " << (A.nzmax) << " non-zeros." << std::endl;
}

void NBLExpSpGEMM::RunImplementationNr(int nr, int threadID)
{
  long countInsert = 0;
  long countOkTryRemove = 0;
  long countEmptyTryRemove = 0;
  struct timespec cur_time;

  handle_t* handle = ThreadInitImplementationNr(nr);

  int myId=(int)threadID;

  switch (mmalg) {
  case 0:
    if (!FAA32(&phase1, 1)) {
      // Sequential in one thread.
      C = SpMM_Gustavson_RowStore(A, A);
    }
    break;

  case 1:
    // New(?) SpMM_DSParallel_RowStore algorithm by Anders Gidenstam, inspired
    // by the classical sequential algorithm in [Gustavson, ACM TMS 4(3), 1978].

    // The SpMM_DSParallel_RowStore algorithm goes in 3 phases.

    // Parallel phase 1.
    {
      SpMM_DSParallel_RowStore_1P(A, A, handle, &nextci, element_count,
                                  countInsert);
      FAA32(&phase1, 1);
      // Wait for all.
#ifndef SPGEMM_USE_MUTEX
      while (phase1 < NR_CPUS) {
        usleep(0);
      }
#else
      pthread_mutex_lock(&phase2_mutex);
      // Let the last thread pass.
      while (phase1 < NR_CPUS) {
        pthread_cond_wait(&phase2_condition, &phase2_mutex);
      }
#endif
    }
    
    // Sequential phase 2.
    {
      int p2 = phase2;
      if (!p2) {
        p2 = FAA32(&phase2, 1);
      }
      if (!p2) {
        // Perform the sequential phase 2. Done by the first thread through.
        clock_gettime(CLOCK_REALTIME, &cur_time);
        phase2_start = ((long double)cur_time.tv_sec +
                        1e-9 * (long double)cur_time.tv_nsec);

        SpMM_DSParallel_RowStore_2S(C, element_count);
        
        // Launch phase 3.
#ifndef SPGEMM_USE_MUTEX
        FAA32(&phase3, 1);
#else
        pthread_mutex_unlock(&phase2_mutex);
        pthread_cond_broadcast(&phase2_condition);
#endif
        clock_gettime(CLOCK_REALTIME, &cur_time);
        phase3_start = ((long double)cur_time.tv_sec +
                        1e-9 * (long double)cur_time.tv_nsec);
      } else {
#ifndef SPGEMM_USE_MUTEX
        // Wait for phase 3 to begin.
        while (phase3 == 0) {
          usleep(0);
        }
#else
        // Let the next thread proceed into phase3 too.
        pthread_mutex_unlock(&phase2_mutex);
#endif
      }
    } // end phase 2

    // Parallel phase 3.
    SpMM_DSParallel_RowStore_3P(C, handle, element_count,
                                countOkTryRemove, countEmptyTryRemove);
    break;
  }

  // Update the global operation counters from all/active phases.
  SaveThreadStatistics(countInsert,
                       countOkTryRemove, countEmptyTryRemove);
  delete handle;

  //std::cout << "The result is a " << (C.m) << "x" << (C.n)
  //          << " matrix with " << (C.nzmax) << " non-zeros." << std::endl;
}

string NBLExpSpGEMM::GetStatistics()
{
  std::stringstream ss;
  ss << std::setprecision(std::numeric_limits<long double>::digits10)
     << NBLExpProducerConsumerBase::GetStatistics()
     << " " << matrix
     << " " << mmalg
     << " " << A.m
     << " " << WUSize
     << " " << phase2_start
     << " " << phase3_start;
  return ss.str();
}

string NBLExpSpGEMM::GetStatisticsLegend()
{
  std::stringstream ss;
  ss << NBLExpProducerConsumerBase::GetStatisticsLegend()
     << " <matrix no.>"
     << " <mm algorithm>"
     << " <square matrix dimension N>"
     << " <work unit size>"
     << " <phase 2 start time>"
     << " <phase 3 start time>";
  return ss.str();
}

void NBLExpSpGEMM::SpMM_DSParallel_RowStore_1P(const SpMatrix& A,
                                               const SpMatrix& B,
                                               handle_t* Ci_bag,
                                               volatile int* nextci,
                                               int* element_count,
                                               long& countInsert)
{
  SparseAccumulator SPA(B.n);
  int ci;
#ifdef SPGEMM_DEBUG
  unsigned long cycles1, cycles2;
#endif
  while (1) {
#ifdef SPGEMM_DEBUG
    start_cycle_count(cycles1);
#endif

    ci = FAA32(nextci, WUSize);
    int cistart = ci;
    int ciend = std::min(ci+WUSize, A.m);

    if (!(ci < A.m)) {
      break;
    }

    SpMatrix::MatrixRow_t** wu =
      (SpMatrix::MatrixRow_t**)std::malloc(WUSize *
                                           sizeof(SpMatrix::MatrixRow_t*));
    int r = 0;

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
      std::vector<int>::const_iterator cik = SPA.nzs.begin();
      std::vector<int>::const_iterator cend  = SPA.nzs.end();
      int nnz = 0;
      wu[r] = new SpMatrix::MatrixRow_t(ci, cend - cik);
      for (; cik < cend; ++cik) {
        double vcik = SPA.v[*cik];
        if (vcik != 0.0) { // FIXME: cut off very small values too?
          wu[r]->ci[nnz] = *cik;
          wu[r]->v[nnz]  = vcik;
          nnz++;
        }
      }
      // Save the true #nz.
      element_count[ci] = nnz;
      r++;
      SPA.Clear();
    }

    // Clear any leftover space in wu.
    for (; r < WUSize; r++) {
      wu[r] = NULL;
    }
#ifdef SPGEMM_DEBUG
    stop_cycle_count(cycles2);
    row_P1_cycles[cistart] = cycles2 - cycles1;
#endif
    // Enqueue the work unit.
    Insert(Ci_bag, wu, countInsert);
  }
}

void NBLExpSpGEMM::SpMM_DSParallel_RowStore_2S(SpMatrix& C, int* element_count)
{
  // Prepare the row pointers.
  int nnz = 0;
  int ci;
  for (ci = 0; ci < C.m; ++ci) {
    C.rp[ci] = nnz;
    nnz += element_count[ci];
  }
  C.rp[ci] = nnz;

  //std::cout << "The result has " << nnz << " non-zeros." << std::endl;

  // Reallocate the ci and v vectors in C.
  C.nzmax = nnz;
  std::free(C.ci);
  std::free(C.v);
  C.ci = (int*)std::malloc(C.nzmax * sizeof(int));
  C.v  = (double*)std::malloc(C.nzmax * sizeof(double));
}

void NBLExpSpGEMM::SpMM_DSParallel_RowStore_3P(SpMatrix& C,
                                               handle_t* Ci_bag,
                                               int* element_count,
                                               long& countOkTryRemove,
                                               long& countEmptyTryRemove)
{
  SpMatrix::MatrixRow_t** wu;
  SpMatrix::MatrixRow_t*  ci;
#ifdef SPGEMM_DEBUG
  unsigned long cycles1, cycles2;
#endif
  while (wu = (SpMatrix::MatrixRow_t**)TryRemove(Ci_bag,
                                                 countOkTryRemove,
                                                 countEmptyTryRemove)) {
#ifdef SPGEMM_DEBUG
    start_cycle_count(cycles1);
    int row = wu[0]->row;
#endif
    for (int r = 0; r < WUSize && wu[r]; r++) {
      ci = wu[r];
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
    std::free(wu);
#ifdef SPGEMM_DEBUG
    stop_cycle_count(cycles2);
    row_P3_cycles[row] = cycles2 - cycles1;
#endif
  }
}

static bool VerifySpMMSquare(const SpMatrix A, const SpMatrix C)
{
  return false;
}
