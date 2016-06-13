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
#pragma once
#include "NBLExpProducerConsumerBase.h"

#include <SpMatrix.h>
#ifdef SPGEMM_USE_MUTEX
#include <pthread.h>
#endif

class NBLExpSpGEMM : public NBLExpProducerConsumerBase
{
public:
  NBLExpSpGEMM(void);
  ~NBLExpSpGEMM(void);
  virtual string GetExperimentName();
  virtual string GetCaseName();
  virtual vector<string> GetParameters();
  virtual vector<string> GetParameterValues(int pno);
  virtual int  GetParameter(int pno);
  virtual void SetParameter(int pno, int value);
  virtual void CreateScenario(); // Random scenario
  virtual void InitImplementationNr(int nr);
  virtual void RunImplementationNr(int nr, int threadID); // For each thread
  virtual string GetStatistics();
  virtual string GetStatisticsLegend();

protected:
  void SpMM_DSParallel_RowStore_1P(const SpMatrix& A, const SpMatrix& B,
                                   handle_t* Ci_bag,
                                   volatile int* nextci, int* element_count,
                                   long& countInsert);
  void SpMM_DSParallel_RowStore_2S(SpMatrix& C, int* element_count);
  void SpMM_DSParallel_RowStore_3P(SpMatrix& C,
                                   handle_t* Ci_bag,
                                   int* element_count,
                                   long& countOkTryRemove,
                                   long& countEmptyTryRemove);

  // Experiment setup.
  //   Work unit size in #rows.
  int WUSize;
  // Experiment control and result collection.
  int matrix;
  int mmalg;
  SpMatrix A;
  SpMatrix C;
  volatile int phase1;
  volatile int phase2;
  long double  phase2_start;
  volatile int phase3;
  long double  phase3_start;
  volatile int nextci;
  int* element_count;
#ifdef SPGEMM_USE_MUTEX
  pthread_mutex_t phase2_mutex;
  pthread_cond_t  phase2_condition;
#endif
#ifdef SPGEMM_DEBUG
  unsigned long* row_P1_cycles;
  unsigned long* row_P3_cycles;
#endif
};
