// Matrix-matrix multiplication benchmark for the experiment framework.
// Copyright (C) 2014 - 2015  Anders Gidenstam
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
#ifdef USE_BLAS

#include "NBLExpGEMM.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>

#include <cblas.h>

#include "primitives.h"

struct Block {
  int row, column, n;

  Block(int row, int column, int n)
  {
    this->row = row;
    this->column = column;
    this->n = n;
  }
};

static void CopyBlockFrom(float *S, int Ns, int row, int column,
                          float *B, int Nb);
static void CopyBlockTo(float *B, int Nb,
                        float *D, int Nd, int row, int column);
static int  VerifyGEMM(float *A, float *B, float *C, float *Src, int n);


NBLExpGEMM::NBLExpGEMM(void)
{
  N   = 8*1024;
  Nwu = 1024;
}

NBLExpGEMM::~NBLExpGEMM(void)
{
  /*
  std::cout << "Verifying correctness of the answer  ... ";
  if (VerifyGEMM(A, B, C, Src, N)) {
    std::cout << "OK!";
  } else {
    std::cout << "FAILED!";
  }
  std::cout << std::endl;
  */

  delete A;
  delete B;
  delete C;
  delete Src;
}

string NBLExpGEMM::GetExperimentName()
{
  return string("GEMM");
}

string NBLExpGEMM::GetCaseName()
{
  std::stringstream ss;
  ss << "-N" << N
     << "-Nwu" << Nwu;
  return ss.str();
}

vector<string> NBLExpGEMM::GetParameters()
{
  vector<string> v;
  v.push_back(string("Matrix size"));
  v.push_back(string("Block (work unit) size"));
  return v;
}


vector<string> NBLExpGEMM::GetParameterValues(int pno)
{
  vector<string> v;
  switch (pno) {
  case 0:
    v.push_back(string("NxN matrix dimension N (default 8192)"));
    break;
  case 1:
    v.push_back(string("Nwu*Nwu block dimension Nwu (default 1024)"));
    break;
  }
  return v;
}

int NBLExpGEMM::GetParameter(int pno)
{
  switch (pno) {
  case 0:
    return N;
  case 1:
    return Nwu;
  default:
    return -1;
  }
}

void NBLExpGEMM::SetParameter(int pno, int value)
{
  switch (pno) {
  case 0:
    N=value;
    break;
  case 1:
    Nwu=value;
  default:
    break;
  }
}

void NBLExpGEMM::CreateScenario()
{
  int i;
  srand((unsigned int)time(NULL));

  // Allocate and initialize the matrices A, B and C.
  A = (float*)calloc(N*N, sizeof(float));
  B = (float*)calloc(N*N, sizeof(float));
  C = (float*)calloc(N*N, sizeof(float));                     
  Src = (float*)calloc(N*N, sizeof(float));                     

  for (i = 0; i < N*N; i++) {
    A[i] = (float)rand()/(float)RAND_MAX - 0.5;
    B[i] = (float)rand()/(float)RAND_MAX - 0.5;
    C[i] = (float)rand()/(float)RAND_MAX - 0.5;
    Src[i] = C[i];
  }
}

void NBLExpGEMM::InitImplementationNr(int nr)
{
  NBLExpProducerConsumerBase::InitImplementationNr(nr);
  added = 0;
}

void NBLExpGEMM::RunImplementationNr(int nr, int threadID)
{
  long countInsert = 0;
  long countOkTryRemove = 0;
  long countEmptyTryRemove = 0;

  handle_t* handle = ThreadInitImplementationNr(nr);

  int myId=(int)threadID;
  int i=0;

  if (NR_CPUS == 1) {
    // Use BLAS directly.
    cblas_sgemm(CblasRowMajor,
                CblasNoTrans, CblasNoTrans,  // for A and B
                N, N, N,                     // m, n, k
                1.0, A, N,                   // alpha, A, LDA
                B, N,                        // B, LDB
                1.0, C, N);                  // beta, C, LDC
  } else {
    // Initiate the suboperations.
    // FIXME: This is rather contention inducing. Can it be done differently?
    // NOTE: beta MUST be 1.0 for this decomposition to work.
    for (int r = myId*Nwu; r < N; r += NR_CPUS*Nwu) {
      for (int c = 0; c < N; c += Nwu) {
        Block *b = new Block(r, c, Nwu);
        Insert(handle, (void*)b, countInsert);
      }
    }
    FAA32(&added, 1);

    // Act as worker.
    float *tempA = (float*)calloc(Nwu*Nwu, sizeof(float));
    float *tempB = (float*)calloc(Nwu*Nwu, sizeof(float));
    float *tempC = (float*)calloc(Nwu*Nwu, sizeof(float));
    int exiting = 0;
    while (1) {
      Block *work = (Block*)TryRemove(handle,
                                      countOkTryRemove,
                                      countEmptyTryRemove);
      if (work) {
        // The destination block will be computed by multiplying one
        // row of blocks from A with a column of blocks from B.
        /*
        std::cout << "Got block(" << work->row 
                  << ", " << work->column
                  << ", " << work->n
                  << ")." << std::endl;
        */

        CopyBlockFrom(C, N, work->row, work->column, tempC, Nwu);
        for (i = 0; i < N; i += Nwu) {
          CopyBlockFrom(A, N, work->row, i, tempA, Nwu);
          CopyBlockFrom(B, N, i, work->column, tempB, Nwu);

          // Use BLAS for the block.
          cblas_sgemm(CblasRowMajor,
                      CblasNoTrans, CblasNoTrans,  // for A and B
                      Nwu, Nwu, Nwu,               // m, n, k
                      1.0, tempA, Nwu,             // alpha, A, LDA
                      tempB, Nwu,                  // B, LDB
                      1.0, tempC, Nwu);            // beta, C, LDC       
        }
        // This block is done.
        CopyBlockTo(tempC, Nwu, C, N, work->row, work->column);
        delete work;

      } else if (added == NR_CPUS) {
        // If all collections were linearizable only one extra
        // iteration would be needed before exiting.
        if (exiting > 10) {
          break;
        }
        exiting++;
      }
    }
    delete tempA;
    delete tempB;
    delete tempC;
  }

  // Update the global operation counters from all/active phases.
  SaveThreadStatistics(countInsert,
                       countOkTryRemove, countEmptyTryRemove);
  delete handle;
}

string NBLExpGEMM::GetStatistics()
{
  std::stringstream ss;
  ss << NBLExpProducerConsumerBase::GetStatistics()
     << " " << N
     << " " << Nwu;
  return ss.str();
}

string NBLExpGEMM::GetStatisticsLegend()
{
  std::stringstream ss;
  ss << NBLExpProducerConsumerBase::GetStatisticsLegend()
     << " <matrix dimension N>"
     << " <block dimension Nwu>";
  return ss.str();
}

static void CopyBlockFrom(float *S, int Ns, int row, int column,
                          float *B, int Nb)
{
  for (int r = 0; r < Nb; r++) {
    memcpy((void*)(B + r*Nb),
           (void*)(S+(row+r)*Ns + column),
           Nb*sizeof(float));
  }
}

static void CopyBlockTo(float *B, int Nb,
                        float *D, int Nd, int row, int column)
{
  for (int r = 0; r < Nb; r++) {
    memcpy((void*)(D+(row+r)*Nd + column),
           (void*)(B + r*Nb),
           Nb*sizeof(float)); 
  }
}

static int  VerifyGEMM(float *A, float *B, float *C, float* Src, int n)
{
  // Use BLAS to compute the right answer in Src.
  cblas_sgemm(CblasRowMajor,
              CblasNoTrans, CblasNoTrans,  // for A and B
              n, n, n,                     // m, n, k
              1.0, A, n,                   // alpha, A, LDA
              B, n,                        // B, LDB
              1.0, Src, n);                // beta, C, LDC

  for (int i = 0; i < n*n; i++) {
    if (fabs(C[i] - Src[i]) > 0.01) {
      return 0;
    }
  }
  return 1;
}

#endif
