// Base class for Dictionary experiments for the experiment framework.
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
#include "NBLExperiment.h"

#include <cmath>

#ifdef USE_NOBLE
#include "Noble.h"
#endif

#ifdef USE_TBB
#include <tbb/concurrent_hash_map.h>
#endif

#ifdef USE_ETL_CBTREE
namespace c_cbtree {
extern "C" {
// The CBTree header includes too many things.
typedef struct node *cbtree_t;
typedef void* key_t;
typedef void* value_t;

cbtree_t* cbtree_alloc();
int search_par(cbtree_t root, key_t key);
value_t get_par(cbtree_t root, key_t key);
int delete_par(cbtree_t root, key_t key);
int insert_par(cbtree_t* root, key_t key, value_t value);
void destroy_tree_nodes(cbtree_t root);
}
}
#endif

#ifdef USE_CCKHT
#include <cstdio>
#include <cstdlib>
namespace cckht {
#include <concckhashtable.h>
#include <BucketizeConcCK.h>
}
#endif

#ifdef USE_HSHT
#define INTEL64 1
#include <HSHashtable.h>
#undef INTEL64
#undef CAS64
#undef CAS32
#endif

// Abstract base class for experiments that use Dictionary collections.
class NBLExpDictionaryBase :
  public NBLExperiment
{
public:
  static const int MAX_CPUS = 40;

  NBLExpDictionaryBase(void);

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
                                    long countOkTryRemove,
                                    long countNotFoundTryRemove);

  typedef void NBLHandle;
  // FIXME:
  //   The interface here needs to be useful and uniform across the
  //   implementations. Currently, it is stronger than what many
  //   implementations provide.
  //   The current set of (expected) operations:
  //   * bool insert(key, value)
  //   * bool lookup(key, out value)
  //   * bool remove(key, out value)
  //   A minimal but much less useable set:
  //   * void insert(key, value)
  //   * bool lookup(key, out value)
  //   * void remove(key)
  typedef bool (*InsertFunc)(NBLHandle *handle, int key, void *value,
                             long& count);
  typedef bool (*LookupFunc)(NBLHandle *handle, int key, void*& value,
                             long& countOk, long& countNotFound);
  typedef bool (*TryRemoveFunc)(NBLHandle *handle, int key, void*& value,
                                long& countOk, long& countNotFound);
  typedef void (*FreeHandleFunc)(NBLHandle *handle);

  virtual NBLHandle *ThreadInitImplementationNr(int nr);

  // Experiment shared state.
  int NR_CPUS;
  int MAX_DICTIONARY_SIZE;

  volatile long long countInsert;
  volatile long long countOkLookup;
  volatile long long countNotFoundLookup;
  volatile long long countOkTryRemove;
  volatile long long countNotFoundTryRemove;

private:
#ifdef USE_NOBLE
  //NBLSkipListRoot *skiplist;
#endif
#ifdef USE_TBB
  tbb::concurrent_hash_map<int, void *> *tbbhashmap;
#endif
#ifdef USE_ETL_CBTREE
  c_cbtree::cbtree_t* cbsearchtree;
#endif
#ifdef USE_CCKHT
  cckht::ConcCukooHashtable<int, void *, cckht::HASH_INT, cckht::HASH_INT1> *cckhashtable;
  cckht::BucketizeConcCK<int, void *>    *bcckhashtable;  
#endif
#ifdef USE_HSHT
  hsht::HSBitmapHopscotchHashMap_t *hsbhhashtable;
  hsht::HSChainedHashMap_t *hschashtable;
  hsht::HSHopscotchHashMap_t *hshhashtable;
#endif

protected: 
  // For the currently configured dictionary collection. 
  InsertFunc Insert;
  TryRemoveFunc Lookup;
  TryRemoveFunc TryRemove;
  FreeHandleFunc FreeHandle;
};
