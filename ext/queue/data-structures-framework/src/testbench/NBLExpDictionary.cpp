// Dictionary microbenchmark for the experiment framework.
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

#include "NBLExpDictionary.h"

#include "primitives.h"

#include <sstream>
#include <iomanip>
#include <limits>
#include <cstdlib>
#include <ctime>

//using namespace tbb;

#define _mm_pause()  asm volatile ("rep; nop" : : )
#define PAUSE _mm_pause()
#define PAUSE_BUNCH PAUSE;PAUSE;PAUSE;PAUSE;PAUSE;PAUSE;PAUSE;PAUSE;PAUSE;PAUSE
#define BIG_PAUSE_BUNCH PAUSE_BUNCH;PAUSE_BUNCH;PAUSE_BUNCH;PAUSE_BUNCH;PAUSE_BUNCH;PAUSE_BUNCH;PAUSE_BUNCH;PAUSE_BUNCH;PAUSE_BUNCH

#define parallel_work()\
  do {                                          \
    unsigned long trash;                        \
    start_cycle_count(trash);                   \
    for(int j=0;j<PARALLEL_WORK;j++){           \
      PAUSE_BUNCH;                              \
    }                                           \
    stop_cycle_count(trash);                    \
  } while(0)

extern volatile int mainCounter;

NBLExpDictionary::NBLExpDictionary(void)
{
  KEY_SPACE = 1024;
  INITIAL_SIZE = 512;
  MAX_DICTIONARY_SIZE = 4*INITIAL_SIZE;
  UPDATE_PERCENTAGE = 20;
  PARALLEL_WORK = 1;
  initialState = 0;
  active = 0;
}

NBLExpDictionary::~NBLExpDictionary(void)
{
  delete initialState;
}

string NBLExpDictionary::GetExperimentName()
{
  return string("Dictionary");
}

string NBLExpDictionary::GetCaseName()
{
  std::stringstream ss;
  ss << "-keyspace" << KEY_SPACE
     << "-isize"    << INITIAL_SIZE
     << "-update"   << UPDATE_PERCENTAGE << "%"
     << "-pw"       << PARALLEL_WORK;
  return ss.str();
}

vector<string> NBLExpDictionary::GetParameters()
{
  vector<string> v;
  v.push_back(string("Key space size"));
  v.push_back(string("Initial dictionary size"));
  v.push_back(string("Update percentage"));
  v.push_back(string("Parallel work size"));
  return v;
}


vector<string> NBLExpDictionary::GetParameterValues(int pno)
{
  vector<string> v;
  switch (pno) {
  case 0:
    v.push_back(string("n for key space size 2^n (integer >=0, default 10)"));
    break;
  case 1:
    v.push_back(string("#key-value pairs (integer >=0, default 512)"));
    break;
  case 2:
    v.push_back(string("update percentage (integer 0-100, default 20)"));
    break;
  case 3:
    v.push_back(string("#parallel work units (integer >=0, default 1)"));
    break;
  }
  return v;
}

int NBLExpDictionary::GetParameter(int pno)
{
  switch (pno) {
  case 0:
    return KEY_SPACE;
  case 1:
    return INITIAL_SIZE;
  case 2:
    return UPDATE_PERCENTAGE;
  case 3:
    return PARALLEL_WORK;
  default:
    return -1;
  }
}

void NBLExpDictionary::SetParameter(int pno, int value)
{
  switch (pno) {
  case 0:
    KEY_SPACE = 1<<value;
    break;
  case 1:
    INITIAL_SIZE = value;
    MAX_DICTIONARY_SIZE = 2*value;
    break;
  case 2:
    UPDATE_PERCENTAGE = value;
    break;
  case 3:
    PARALLEL_WORK = value;
    break;
  default:
    break;
  }
}

void NBLExpDictionary::CreateScenario()
{
  srand((unsigned int)time(NULL));

  delete initialState;
  initialState = new std::map<int,void*>();
  for (int i=0 ; i < INITIAL_SIZE; i++) {
    int   key = (rand() % KEY_SPACE) + 1;
    void* value = (void*)key;
    initialState->insert(std::pair<int,void*>(key, value));
  }

  for (int t=0; t < MAX_CPUS; t++) {
    for (int i=0; i < MAX_OPS; i++) {
      opSchedule[t][i].first =
        (rand() < RAND_MAX*(double)UPDATE_PERCENTAGE/100.0) ? UPDATE :  LOOKUP;
      opSchedule[t][i].second = (rand() % KEY_SPACE) + 1;
    }
  }
}

void NBLExpDictionary::InitImplementationNr(int nr)
{
  NBLExpDictionaryBase::InitImplementationNr(nr);

  long countInsert = 0;
  long countOkLookup = 0;
  long countNotFoundLookup = 0;
  long countOkTryRemove = 0;
  long countNotFoundTryRemove = 0;

  NBLHandle *handle = ThreadInitImplementationNr(nr);
  for (std::map<int,void*>::iterator it = initialState->begin();
       it != initialState->end();
       ++it) {
    Insert(handle, it->first, it->second,
           countInsert);
  }
  FreeHandle(handle);
}

void NBLExpDictionary::RunImplementationNr(int nr, int threadID)
{
  long countInsert = 0;
  long countOkLookup = 0;
  long countNotFoundLookup = 0;
  long countOkTryRemove = 0;
  long countNotFoundTryRemove = 0;

  NBLHandle *handle = ThreadInitImplementationNr(nr);

  long i = 0;
  bool updating = false;
  while (mainCounter) {
    int op  = opSchedule[threadID][i % MAX_OPS].first;
    int key = opSchedule[threadID][i % MAX_OPS].second;
    void *value;

    switch (op) {
    case LOOKUP:
      Lookup(handle, key, value, countOkLookup, countNotFoundLookup);
      i++;
      break;
    case UPDATE:
      if (!updating) {
        if (TryRemove(handle, key, value,
                      countOkTryRemove, countNotFoundTryRemove)) {
          updating = true;
        } else {
          i++;
        }
      } else {
        updating = false;
        Insert(handle, key, (void*)key, countInsert);
        i++;
      }
      break;
    }

    parallel_work();
  }

  // Update the global operation counters from all/active phases.
  SaveThreadStatistics(countInsert,
                       countOkLookup, countNotFoundLookup,
                       countOkTryRemove, countNotFoundTryRemove);
  FreeHandle(handle);
}

string NBLExpDictionary::GetStatistics()
{
  std::stringstream ss;
  ss << NBLExpDictionaryBase::GetStatistics()
     << " " << KEY_SPACE
     << " " << INITIAL_SIZE
     << " " << UPDATE_PERCENTAGE
     << " " << PARALLEL_WORK;
  return ss.str();
}

string NBLExpDictionary::GetStatisticsLegend()
{
  std::stringstream ss;
  ss << NBLExpDictionaryBase::GetStatisticsLegend()
     << " <key space>"
     << " <initial dictionary size>"
     << " <update percentage>"
     << " <parallel work>";
  return ss.str();
}
