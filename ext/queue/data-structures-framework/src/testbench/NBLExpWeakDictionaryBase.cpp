// Base class for weak dictionary experiments for the experiment framework.
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
#include "NBLExpWeakDictionaryBase.h"

#include <sstream>
#include <iostream>

#include <cstdlib>

#include "primitives.h"

#ifdef USE_NOBLE
#define NOBLE_STATUS " (unavailable)"
#else
#define NOBLE_STATUS " (unavailable)"
#endif

#ifdef USE_TBB
#define TBB_STATUS
#else
#define TBB_STATUS " (unavailable)"
#endif

#ifdef USE_ETL
#define ETL_STATUS
#else
#define ETL_STATUS " (unavailable)"
#endif

#ifdef USE_CCKHT
#define CCKHT_STATUS
#else
#define CCKHT_STATUS " (unavailable)"
#endif

#ifdef USE_HSHT
#define HSHT_STATUS
#else
#define HSHT_STATUS " (unavailable)"
#endif

NBLExpWeakDictionaryBase::NBLExpWeakDictionaryBase(void)
{
  NR_CPUS=2;
}

void NBLExpWeakDictionaryBase::SetNrThreads(int nrOfThreads)
{
  NR_CPUS = std::min(nrOfThreads, NBLExpWeakDictionaryBase::MAX_CPUS);
}

vector<string> NBLExpWeakDictionaryBase::GetImplementations()
{
  vector<string> v;
  v.push_back(string("Dictionary NOBLE SkipList" NOBLE_STATUS));
  v.push_back(string("Dictionary TBB concurrent_hash_map" TBB_STATUS));
#ifdef USE_ETL
#ifdef USE_ETL_CBTREE
  v.push_back(string("Dictionary ETL CBTree" ETL_STATUS));
#endif
#ifdef USE_ETL_DELTATREE
  v.push_back(string("Dictionary ETL DeltaTree" ETL_STATUS));
#endif
#ifdef USE_ETL_GREENBST
  v.push_back(string("Dictionary ETL GreenBST" ETL_STATUS));
#endif
#else
  v.push_back(string("Dictionary ETL *" ETL_STATUS));
#endif
  v.push_back(string("Dictionary CCKHT Concurrent Cuckoo hash table"
                     CCKHT_STATUS));
  v.push_back(string("Dictionary CCKHT Bucketized Concurrent Cuckoo hash table"
                     CCKHT_STATUS));
  v.push_back(string("Dictionary HS bitmap hopscotch hash table"
                     HSHT_STATUS));
  v.push_back(string("Dictionary HS chained hash table"
                     HSHT_STATUS));
  v.push_back(string("Dictionary HS hopscotch hash table"
                     HSHT_STATUS));
  return v;
}

void NBLExpWeakDictionaryBase::InitImplementationNr(int nr)
{
  countInsert = 0;
  countOkLookup = 0;
  countNotFoundLookup = 0;
  countRemove = 0;

  switch(nr) {
  case 0:
#ifdef USE_NOBLE
#else
#endif
    std::cerr << "Error: Compiled without NOBLE support!" << std::endl;
    exit(-1);
    break;
  case 1:
#ifdef USE_TBB
    weak_dictionary =
      new excess::concurrent_weak_dictionary_TBBhashmap<int,void>();
#else
    std::cerr << "Error: Compiled without Intel TBB support!" << std::endl;
    exit(-1);
#endif
    break;
  case 2:
#ifdef USE_ETL
#ifdef USE_ETL_CBTREE
    weak_dictionary =
      new excess::concurrent_weak_dictionary_CBTree<int,void>();
#endif
#ifdef USE_ETL_DELTATREE
    weak_dictionary =
      new excess::concurrent_weak_dictionary_DeltaTree<int,void>();
#endif
#ifdef USE_ETL_GREENBST
    weak_dictionary =
      new excess::concurrent_weak_dictionary_GreenBST<int,void>();
#endif
#else
    std::cerr << "Error: Compiled without EXCESS Tree Library support!"
              << std::endl;
    exit(-1);
#endif
    break;
  case 3:
#ifdef USE_CCKHT
    weak_dictionary =
      new excess::concurrent_weak_dictionary_CCKHT<int,void>(MAX_DICTIONARY_SIZE);
#else
    std::cerr << "Error: Compiled without CCKHT support!" << std::endl;
    exit(-1);
#endif
    break;
  case 4:
#ifdef USE_CCKHT
    weak_dictionary =
      new excess::concurrent_weak_dictionary_BCCKHT<int,void>(MAX_DICTIONARY_SIZE);
#else
    std::cerr << "Error: Compiled without CCKHT support!" << std::endl;
    exit(-1);
#endif
    break;
  case 5:
#ifdef USE_HSHT
    weak_dictionary =
      new excess::concurrent_weak_dictionary_HSHTBH<int,void>
            (MAX_DICTIONARY_SIZE, MAX_CPUS);
#else
    std::cerr << "Error: Compiled without HSHT support!" << std::endl;
    exit(-1);
#endif
    break;
  case 6:
#ifdef USE_HSHT
    weak_dictionary =
      new excess::concurrent_weak_dictionary_HSHTC<int,void>
            (MAX_DICTIONARY_SIZE, MAX_CPUS);
#else
    std::cerr << "Error: Compiled without HSHT support!" << std::endl;
    exit(-1);
#endif
    break;
  case 7:
#ifdef USE_HSHT
    weak_dictionary =
      new excess::concurrent_weak_dictionary_HSHTH<int,void>
            (MAX_DICTIONARY_SIZE, MAX_CPUS);
#else
    std::cerr << "Error: Compiled without HSHT support!" << std::endl;
    exit(-1);
#endif
    break;
  }
}

void NBLExpWeakDictionaryBase::WeakDictionaryInsert
  (weak_dictionary_t::handle* handle,
   int key, void* value,
   long& count)
{
  handle->insert(key, value);
  count++;
}

bool NBLExpWeakDictionaryBase::WeakDictionaryLookup
  (weak_dictionary_t::handle* handle,
   int key, void*& value,
   long& countOk, long& countNotFound)
{
  if (handle->lookup(key, value)) {
    countOk++;
    return true;
  } else {
    countNotFound++;
    return false;
  }
}

void NBLExpWeakDictionaryBase::WeakDictionaryRemove
  (weak_dictionary_t::handle* handle,
   int key, void*& value,
   long& count)
{
  handle->remove(key);
  count++;
}

void NBLExpWeakDictionaryBase::WeakDictionaryFreeHandle
  (weak_dictionary_t::handle* handle)
{
  delete handle;
}

NBLExpWeakDictionaryBase::weak_dictionary_t::handle*
NBLExpWeakDictionaryBase::ThreadInitImplementationNr(int nr)
{
  return weak_dictionary->get_handle();
}

void NBLExpWeakDictionaryBase::DeInitImplementationNr(int nr)
{
  delete weak_dictionary;
}

string NBLExpWeakDictionaryBase::GetStatistics()
{
  std::stringstream ss;
  ss << countInsert << "  "
     << countOkLookup << " "
     << countNotFoundLookup << " "
     << countRemove;
  return ss.str();
}

string NBLExpWeakDictionaryBase::GetStatisticsLegend()
{
  std::stringstream ss;
  ss << "<#Insert operations>"
     << " <#Ok Lookup operations>"
     << " <#NotFound Lookup operations>"
     << " <#Remove operations>";
  return ss.str();
}

void NBLExpWeakDictionaryBase::SaveThreadStatistics(long countInsert,
                                                    long countOkLookup,
                                                    long countNotFoundLookup,
                                                    long countRemove)
{
  // Update the global operation counters.
  STDFAA(&this->countInsert, countInsert);
  STDFAA(&this->countOkLookup, countOkLookup);
  STDFAA(&this->countNotFoundLookup, countNotFoundLookup);
  STDFAA(&this->countRemove, countRemove);
}
