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
#include "NBLExpDictionaryBase.h"

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

typedef tbb::concurrent_hash_map<int,void*> tbb_hash_map_t;
#else
#define TBB_STATUS " (unavailable)"
#endif

#ifdef USE_ETL_CBTREE
#define ETL_STATUS
#else
#define ETL_STATUS " (unavailable)"
#endif

#ifdef USE_CCKHT
#define CCKHT_STATUS

typedef cckht::ConcCukooHashtable<int,void*,cckht::HASH_INT,cckht::HASH_INT1> cckht_cck_hash_map_t;
typedef cckht::BucketizeConcCK<int,void*>  cckht_bcck_hash_map_t;
#else
#define CCKHT_STATUS " (unavailable)"
#endif

#ifdef USE_HSHT
#define HSHT_STATUS
#else
#define HSHT_STATUS " (unavailable)"
#endif


typedef void NBLHandle;

#ifdef USE_NOBLE
#endif

#ifdef USE_TBB
static bool TBBHashMapInsert(NBLHandle *handle,
                             int key, void *value,
                             long& count)
{
  bool ret = static_cast<tbb_hash_map_t*>(handle)->
    insert(std::pair<int,void*>(key,value));
  count++;
  return ret;
}
static bool TBBHashMapLookup(NBLHandle *handle,
                             int key, void*& value,
                             long& countOk, long& countNotFound)
{
  //tbb_hash_map_t::const_accessor acc;
  if (static_cast<tbb_hash_map_t*>(handle)->count(key)) {
    countOk++;
    // FIXME: Set the out parameter. Will need to use find().
    return true;
  } else {
    countNotFound++;
    return false;
  }
}
static bool TBBHashMapTryRemove(NBLHandle *handle,
                                int key, void*& value,
                                long& countOk, long& countNotFound)
{
  if (static_cast<tbb_hash_map_t*>(handle)->erase(key)) {
    countOk++;
    // FIXME: Set the out parameter. Will need to use another erase().
    return true;
  } else {
    countNotFound++;
    return false;
  }
  /*
  tbb_hash_map_t::const_accessor acc;
  if (static_cast<tbb_hash_map_t*>(handle)->find(acc, key)) {
    static_cast<tbb_hash_map_t*>(handle)->erase(acc);
    countOk++;
    // FIXME: Set the out parameter.
    return true;
  } else {
    countNotFound++;
    return false;
  }
  */
}
static void TBBHashMapFreeHandle(void *handle)
{
}
#endif

#ifdef USE_ETL_CBTREE
static bool ETLCBTreeInsert(NBLHandle *handle,
                            int key, void *value,
                            long& count)
{
  int ret = c_cbtree::insert_par(static_cast<c_cbtree::cbtree_t*>(handle),
                                 (c_cbtree::key_t)key, value);
  count++;
  return ret;
}
static bool ETLCBTreeLookup(NBLHandle *handle,
                            int key, void*& value,
                            long& countOk, long& countNotFound)
{
  if (value = c_cbtree::search_par(*static_cast<c_cbtree::cbtree_t*>(handle), (c_cbtree::key_t)key)) {
    countOk++;
    return true;
  } else {
    countNotFound++;
    return false;
  }
}
static bool ETLCBTreeTryRemove(NBLHandle *handle,
                               int key, void*& value,
                               long& countOk, long& countNotFound)
{
  if (c_cbtree::delete_par(*static_cast<c_cbtree::cbtree_t*>(handle),
                           (c_cbtree::key_t)key)) {
    countOk++;
    // FIXME: Set the out parameter. Not supported by the API.
    return true;
  } else {
    countNotFound++;
    return false;
  }
}
static void ETLCBTreeFreeHandle(void *handle)
{
}
#endif

#ifdef USE_CCKHT
static bool CCKHashMapInsert(NBLHandle *handle,
                             int key, void *value,
                             long& count)
{
  bool ret = static_cast<cckht_cck_hash_map_t*>(handle)->
    Insert(key, value);
  count++;
  return ret;
}
static bool CCKHashMapLookup(NBLHandle *handle,
                             int key, void*& value,
                             long& countOk, long& countNotFound)
{
  if (static_cast<cckht_cck_hash_map_t*>(handle)->Search(key, value)) {
    countOk++;
    return true;
  } else {
    countNotFound++;
    return false;
  }
}
static bool CCKHashMapTryRemove(NBLHandle *handle,
                                int key, void*& value,
                                long& countOk, long& countNotFound)
{
  if (true || static_cast<cckht_cck_hash_map_t*>(handle)->Search(key, value)) {
    static_cast<cckht_cck_hash_map_t*>(handle)->Remove(key);
    countOk++;
    // FIXME: The value for the out parameter is not linearizable.
    return true;
  } else {
    countNotFound++;
    return false;
  }
}
static bool BCCKHashMapInsert(NBLHandle *handle,
                              int key, void *value,
                              long& count)
{
  bool ret = static_cast<cckht_bcck_hash_map_t*>(handle)->
    Insert(key, value);
  count++;
  return ret;
}
static bool BCCKHashMapLookup(NBLHandle *handle,
                              int key, void*& value,
                              long& countOk, long& countNotFound)
{
  if (static_cast<cckht_bcck_hash_map_t*>(handle)->Search(key, value)) {
    countOk++;
    return true;
  } else {
    countNotFound++;
    return false;
  }
}
static bool BCCKHashMapTryRemove(NBLHandle *handle,
                                 int key, void*& value,
                                 long& countOk, long& countNotFound)
{
  if (true || static_cast<cckht_bcck_hash_map_t*>(handle)->Search(key, value)) {
    static_cast<cckht_bcck_hash_map_t*>(handle)->Remove(key);
    countOk++;
    // FIXME: The value for the out parameter is not linearizable.
    return true;
  } else {
    countNotFound++;
    return false;
  }
}
static void CCKHashMapFreeHandle(void *handle)
{
}
#endif

#ifdef USE_HSHT
static bool HSBHHashMapInsert(NBLHandle *handle,
                              int key, void *value,
                              long& count)
{
  void* ret = static_cast<hsht::HSBitmapHopscotchHashMap_t*>(handle)->
    putIfAbsent(key, value);
  count++;
  return (ret == 0);
}
static bool HSBHHashMapLookup(NBLHandle *handle,
                              int key, void*& value,
                              long& countOk, long& countNotFound)
{
  if (static_cast<hsht::HSBitmapHopscotchHashMap_t*>(handle)->
          containsKey(key)) {
    // FIXME: value is not set.
    countOk++;
    return true;
  } else {
    countNotFound++;
    return false;
  }
}
static bool HSBHHashMapTryRemove(NBLHandle *handle,
                                 int key, void*& value,
                                 long& countOk, long& countNotFound)
{
  if (0 !=
      (value = static_cast<hsht::HSBitmapHopscotchHashMap_t*>(handle)->remove(key))) {
    countOk++;
    return true;
  } else {
    countNotFound++;
    return false;
  }
}
static bool HSCHashMapInsert(NBLHandle *handle,
                             int key, void *value,
                             long& count)
{
  void* ret = static_cast<hsht::HSChainedHashMap_t*>(handle)->
    putIfAbsent(key, value);
  count++;
  return (ret == 0);
}
static bool HSCHashMapLookup(NBLHandle *handle,
                             int key, void*& value,
                             long& countOk, long& countNotFound)
{
  if (static_cast<hsht::HSChainedHashMap_t*>(handle)->
          containsKey(key)) {
    // FIXME: value is not set.
    countOk++;
    return true;
  } else {
    countNotFound++;
    return false;
  }
}
static bool HSCHashMapTryRemove(NBLHandle *handle,
                                int key, void*& value,
                                long& countOk, long& countNotFound)
{
  if (0 !=
      (value = static_cast<hsht::HSChainedHashMap_t*>(handle)->remove(key))) {
    countOk++;
    return true;
  } else {
    countNotFound++;
    return false;
  }
}
static bool HSHHashMapInsert(NBLHandle *handle,
                             int key, void *value,
                             long& count)
{
  void* ret = static_cast<hsht::HSHopscotchHashMap_t*>(handle)->
    putIfAbsent(key, value);
  count++;
  return (ret == 0);
}
static bool HSHHashMapLookup(NBLHandle *handle,
                             int key, void*& value,
                             long& countOk, long& countNotFound)
{
  if (static_cast<hsht::HSHopscotchHashMap_t*>(handle)->
          containsKey(key)) {
    // FIXME: value is not set.
    countOk++;
    return true;
  } else {
    countNotFound++;
    return false;
  }
}
static bool HSHHashMapTryRemove(NBLHandle *handle,
                                int key, void*& value,
                                long& countOk, long& countNotFound)
{
  if (0 !=
      (value = static_cast<hsht::HSHopscotchHashMap_t*>(handle)->remove(key))) {
    countOk++;
    return true;
  } else {
    countNotFound++;
    return false;
  }
}
static void HSHashMapFreeHandle(void *handle)
{
}
#endif

NBLExpDictionaryBase::NBLExpDictionaryBase(void)
{
  NR_CPUS=2;
}

void NBLExpDictionaryBase::SetNrThreads(int nrOfThreads)
{
  NR_CPUS = std::min(nrOfThreads, NBLExpDictionaryBase::MAX_CPUS);
}

vector<string> NBLExpDictionaryBase::GetImplementations()
{
  vector<string> v;
  v.push_back(string("Dictionary NOBLE SkipList" NOBLE_STATUS));
  v.push_back(string("Dictionary TBB concurrent_hash_map" TBB_STATUS));
  v.push_back(string("Dictionary ETL cbtree_t" ETL_STATUS));
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

void NBLExpDictionaryBase::InitImplementationNr(int nr)
{
  countInsert = 0;
  countOkLookup = 0;
  countNotFoundLookup = 0;
  countOkTryRemove = 0;
  countNotFoundTryRemove = 0;

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
    tbbhashmap = new tbb::concurrent_hash_map<int,void*>();
#else
    std::cerr << "Error: Compiled without Intel TBB support!" << std::endl;
    exit(-1);
#endif
    break;
  case 2:
#ifdef USE_ETL_CBTREE
    cbsearchtree = c_cbtree::cbtree_alloc();
#else
    std::cerr << "Error: Compiled without EXCESS Tree Library support!"
              << std::endl;
    exit(-1);
#endif
    break;
  case 3:
#ifdef USE_CCKHT
    cckhashtable = new cckht_cck_hash_map_t(MAX_DICTIONARY_SIZE);
#else
    std::cerr << "Error: Compiled without CCKHT support!" << std::endl;
    exit(-1);
#endif
    break;
  case 4:
#ifdef USE_CCKHT
    bcckhashtable = new cckht_bcck_hash_map_t(MAX_DICTIONARY_SIZE);
#else
    std::cerr << "Error: Compiled without CCKHT support!" << std::endl;
    exit(-1);
#endif
    break;
  case 5:
#ifdef USE_HSHT
    hsbhhashtable = new hsht::HSBitmapHopscotchHashMap_t(MAX_DICTIONARY_SIZE, MAX_CPUS);
#else
    std::cerr << "Error: Compiled without HSHT support!" << std::endl;
    exit(-1);
#endif
    break;
  case 6:
#ifdef USE_HSHT
    hschashtable = new hsht::HSChainedHashMap_t(MAX_DICTIONARY_SIZE, MAX_CPUS);
#else
    std::cerr << "Error: Compiled without HSHT support!" << std::endl;
    exit(-1);
#endif
    break;
  case 7:
#ifdef USE_HSHT
    hshhashtable = new hsht::HSHopscotchHashMap_t(MAX_DICTIONARY_SIZE, MAX_CPUS);
#else
    std::cerr << "Error: Compiled without HSHT support!" << std::endl;
    exit(-1);
#endif
    break;
  }
  // Set up the collection operations pointers.
  switch(nr) {
  case 0:
#ifdef USE_NOBLE
#endif
    break;
  case 1:
#ifdef USE_TBB
    Insert = TBBHashMapInsert;
    Lookup = TBBHashMapLookup;
    TryRemove = TBBHashMapTryRemove;
    FreeHandle = TBBHashMapFreeHandle;
#endif
    break;
  case 2:
#ifdef USE_ETL_CBTREE
    Insert = ETLCBTreeInsert;
    Lookup = ETLCBTreeLookup;
    TryRemove = ETLCBTreeTryRemove;
    FreeHandle = ETLCBTreeFreeHandle;
#endif
    break;
  case 3:
#ifdef USE_CCKHT
    Insert = CCKHashMapInsert;
    Lookup = CCKHashMapLookup;
    TryRemove = CCKHashMapTryRemove;
    FreeHandle = CCKHashMapFreeHandle;
#endif
    break;
  case 4:
#ifdef USE_CCKHT
    Insert = BCCKHashMapInsert;
    Lookup = BCCKHashMapLookup;
    TryRemove = BCCKHashMapTryRemove;
    FreeHandle = CCKHashMapFreeHandle;
#endif
    break;
  case 5:
#ifdef USE_HSHT
    Insert = HSBHHashMapInsert;
    Lookup = HSBHHashMapLookup;
    TryRemove = HSBHHashMapTryRemove;
    FreeHandle = HSHashMapFreeHandle;
#endif
    break;
  case 6:
#ifdef USE_HSHT
    Insert = HSCHashMapInsert;
    Lookup = HSCHashMapLookup;
    TryRemove = HSCHashMapTryRemove;
    FreeHandle = HSHashMapFreeHandle;
#endif
    break;
  case 7:
#ifdef USE_HSHT
    Insert = HSHHashMapInsert;
    Lookup = HSHHashMapLookup;
    TryRemove = HSHHashMapTryRemove;
    FreeHandle = HSHashMapFreeHandle;
#endif
    break;
  }
}

NBLHandle *NBLExpDictionaryBase::ThreadInitImplementationNr(int nr)
{
  NBLHandle *handle = 0;
  switch(nr) {
  case 0:
#ifdef USE_NOBLE
#endif
    break;
  case 1:
#ifdef USE_TBB
    handle = tbbhashmap;
#endif
    break;
  case 2:
#ifdef USE_ETL_CBTREE
    handle = cbsearchtree;
#endif
    break;
  case 3:
#ifdef USE_CCKHT
    handle = cckhashtable;
#endif
    break;
  case 4:
#ifdef USE_CCKHT
    handle = bcckhashtable;
#endif
    break;
  case 5:
#ifdef USE_HSHT
    handle = hsbhhashtable;
#endif
    break;
  case 6:
#ifdef USE_HSHT
    handle = hschashtable;
#endif
    break;
  case 7:
#ifdef USE_HSHT
    handle = hshhashtable;
#endif
    break;
  }
  return handle;
}

void NBLExpDictionaryBase::DeInitImplementationNr(int nr)
{
  switch(nr) {
  case 0:
#ifdef USE_NOBLE
#endif
    break;
  case 1:
#ifdef USE_TBB
    delete tbbhashmap;
#endif
    break;
  case 2:
#ifdef USE_ETL_CBTREE
    c_cbtree::destroy_tree_nodes(*cbsearchtree);
#endif
    break;
  case 3:
#ifdef USE_CCKHT
    delete cckhashtable;
#endif
  case 4:
#ifdef USE_CCKHT
    delete bcckhashtable;
#endif
    break;
  case 5:
#ifdef USE_HSHT
    delete hsbhhashtable;
#endif
    break;
  case 6:
#ifdef USE_HSHT
    delete hschashtable;
#endif
    break;
  case 7:
#ifdef USE_HSHT
    delete hshhashtable;
#endif
    break;
  }
}

string NBLExpDictionaryBase::GetStatistics()
{
  std::stringstream ss;
  ss << countInsert << "  "
     << countOkLookup << " "
     << countNotFoundLookup << " "
     << countOkTryRemove << " "
     << countNotFoundTryRemove;
  return ss.str();
}

string NBLExpDictionaryBase::GetStatisticsLegend()
{
  std::stringstream ss;
  ss << "<#Insert operations>"
     << " <#Ok Lookup operations>"
     << " <#NotFound Lookup operations>"
     << " <#Ok TryRemove operations>"
     << " <#NotFound TryRemove operations>";
  return ss.str();
}

void NBLExpDictionaryBase::SaveThreadStatistics(long countInsert,
                                                long countOkLookup,
                                                long countNotFoundLookup,
                                                long countOkTryRemove,
                                                long countNotFoundTryRemove)
{
  // Update the global operation counters.
  STDFAA(&this->countInsert, countInsert);
  STDFAA(&this->countOkLookup, countOkLookup);
  STDFAA(&this->countNotFoundLookup, countNotFoundLookup);
  STDFAA(&this->countOkTryRemove, countOkTryRemove);
  STDFAA(&this->countNotFoundTryRemove, countNotFoundTryRemove);
}
