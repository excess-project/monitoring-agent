// Base class for weak dictionary experiments for the experiment framework.
// Copyright (C) 2015  Anders Gidenstam
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
#include "NBLExperiment.h"

#include <cmath>

#include <EXCESS/concurrent_weak_dictionary>

// Abstract base class for experiments that use weak dictionary collections.
class NBLExpWeakDictionaryBase :
  public NBLExperiment
{
public:
  static const int MAX_CPUS = 40;

  NBLExpWeakDictionaryBase(void);

  virtual void SetNrThreads(int nrOfThreads);
  virtual vector<string> GetImplementations();

  virtual void InitImplementationNr(int nr);
  // ThreadInitImplementationNr() must be called by each thread before the
  // experiment commences.
  virtual void DeInitImplementationNr(int nr);
  virtual string GetStatistics();
  virtual string GetStatisticsLegend();
protected:
  virtual void SaveThreadStatistics(long countInsert,
                                    long countOkLookup,
                                    long countNotFoundLookup,
                                    long countRemove);

  typedef excess::concurrent_weak_dictionary<int,void> weak_dictionary_t;
  typedef weak_dictionary_t::handle handle_t;

  //   The weak dictionary interface offers the operations:
  //   * void insert(key, value)
  //   * bool lookup(key, out value)
  //   * void remove(key)
  //   and is not very useful. However, it is the
  //   common ground across the various implementations.
  //
  //   The stronger dictionary interface offers the operations:
  //   * bool insert(key, value)
  //   * bool lookup(key, out value)
  //   * bool remove(key, out value)

  // For the currently configured weak dictionary collection.
  void WeakDictionaryInsert(weak_dictionary_t::handle* handle,
                            int key, void *value,
                            long& count);
  bool WeakDictionaryLookup(weak_dictionary_t::handle* handle,
                            int key, void*& value,
                            long& countOk, long& countNotFound);
  void WeakDictionaryRemove(weak_dictionary_t::handle* handle,
                            int key, void*& value,
                            long& count);
  void WeakDictionaryFreeHandle(weak_dictionary_t::handle* handle);

  virtual handle_t *ThreadInitImplementationNr(int nr);

  // Experiment shared state.
  int NR_CPUS;
  int MAX_DICTIONARY_SIZE;

  volatile long long countInsert;
  volatile long long countOkLookup;
  volatile long long countNotFoundLookup;
  volatile long long countRemove;

private:
  excess::concurrent_weak_dictionary<int, void>* weak_dictionary;
};
