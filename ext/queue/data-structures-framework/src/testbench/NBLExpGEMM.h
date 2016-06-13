// Matrix-matrix multiplication benchmark for the EXCESS experiment framework.
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
#pragma once
#include "NBLExpProducerConsumerBase.h"

class NBLExpGEMM : public NBLExpProducerConsumerBase
{
public:
  NBLExpGEMM(void);
  ~NBLExpGEMM(void);
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
  // Experiment setup.
  //   Matrix dimension (square matrices)
  int N;
  //   Sub-matrix (work unit) size. Should be power of 2.
  int Nwu;
  // Experiment control and result collection.
  float *A, *B, *C, *Src;
  volatile int added;
};
