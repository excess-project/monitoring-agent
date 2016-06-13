// Base class for Producer-Consumer experiments for the experiment framework.
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

#include "NBLExpProducerConsumerBase.h"

#include <sstream>
#include <iostream>

#include <cstdlib>

#include "primitives.h"

#ifdef USE_NOBLE
#define NOBLE_STATUS
#else
#define NOBLE_STATUS " (unavailable)"
#endif

#ifdef USE_TBB
#define TBB_STATUS
#else
#define TBB_STATUS " (unavailable)"
#endif

typedef excess::concurrent_bag<void> concurrent_bag_t;
typedef excess::concurrent_bag<void>::handle handle_t;

static void BagInsert(handle_t* handle, void* argument, long& count)
{
  handle->insert(argument);
  count++;
}

static void* BagTryRemove(handle_t* handle, long& countOk, long& countEmpty)
{
  void* value = 0;
  if (handle->try_remove_any(value)) {
    countOk++;
  } else {
    countEmpty++;
  }
  return value;
}

NBLExpProducerConsumerBase::NBLExpProducerConsumerBase(void)
{
  bag = 0;
  NR_CPUS=2;
  NR_OPERS=500000;
}

void NBLExpProducerConsumerBase::SetNrThreads(int nrOfThreads)
{
  NR_CPUS = std::min(nrOfThreads, NBLExpProducerConsumerBase::MAX_CPUS);
}

vector<string> NBLExpProducerConsumerBase::GetImplementations()
{
  vector<string> v;
  v.push_back(string("Queue NOBLE Lock-Free DB" NOBLE_STATUS));
  v.push_back(string("Queue NOBLE Lock-Free DU" NOBLE_STATUS));
  v.push_back(string("Queue NOBLE Lock-Free SB" NOBLE_STATUS));
  v.push_back(string("Queue NOBLE Lock-Free BB" NOBLE_STATUS));
  v.push_back(string("Queue NOBLE Lock-Free BASKET" NOBLE_STATUS));
  v.push_back(string("Queue NOBLE Lock-Free ELIM" NOBLE_STATUS));
  v.push_back(string("Queue NOBLE Lock-Based" NOBLE_STATUS));
  v.push_back(string("Queue TBB concurrent_queue" TBB_STATUS));
  v.push_back(string("Queue M&S two-lock queue"));
  v.push_back(string("Stack NOBLE Lock-Free B" NOBLE_STATUS));
  v.push_back(string("Stack NOBLE Lock-Free ELIM" NOBLE_STATUS));
  v.push_back(string("Bag NOBLE Lock-Free BB" NOBLE_STATUS));
  v.push_back(string("Pool NOBLE Lock-Free EDTREE" NOBLE_STATUS));
  return v;
}

void NBLExpProducerConsumerBase::InitImplementationNr(int nr)
{
  countInsert = 0;
  countOkTryRemove = 0;
  countEmptyTryRemove = 0;

  switch(nr) {
#ifdef USE_NOBLE
  case 0:
    bag =
      new excess::concurrent_bag_NBLQueue<void>
            (excess::concurrent_bag_NBLQueue<void>::LF_DB,
             NR_OPERS*NBLExpProducerConsumerBase::MAX_CPUS);
    break;
  case 1:
    bag =
      new excess::concurrent_bag_NBLQueue<void>
            (excess::concurrent_bag_NBLQueue<void>::LF_DU,
             NR_OPERS*NBLExpProducerConsumerBase::MAX_CPUS);
    break;
  case 2:
    bag =
      new excess::concurrent_bag_NBLQueue<void>
            (excess::concurrent_bag_NBLQueue<void>::LF_SB,
             NR_OPERS*NBLExpProducerConsumerBase::MAX_CPUS);
    break;
  case 3:
    bag =
      new excess::concurrent_bag_NBLQueue<void>
            (excess::concurrent_bag_NBLQueue<void>::LF_BB,
             NR_OPERS*NBLExpProducerConsumerBase::MAX_CPUS);
    break;
  case 4:
    bag =
      new excess::concurrent_bag_NBLQueue<void>
            (excess::concurrent_bag_NBLQueue<void>::LF_BASKET,
             NR_OPERS*NBLExpProducerConsumerBase::MAX_CPUS);
    break;
  case 5:
    bag =
      new excess::concurrent_bag_NBLQueue<void>
            (excess::concurrent_bag_NBLQueue<void>::LF_ELIM,
             NR_OPERS*NBLExpProducerConsumerBase::MAX_CPUS);
    break;
  case 6:
    bag =
      new excess::concurrent_bag_NBLQueue<void>
            (excess::concurrent_bag_NBLQueue<void>::LB,
             NR_OPERS*NBLExpProducerConsumerBase::MAX_CPUS);
    break;
#endif
  case 7:
#ifdef USE_TBB
    bag = new excess::concurrent_bag_TBBQueue<void>();
#else
    std::cerr << "Error: Compiled without Intel TBB support!" << std::endl;
    exit(-1);
#endif
    break;
  case 8:
    bag =
      new excess::concurrent_bag_MSTLB<void>();
    break;
#ifdef USE_NOBLE
  case 9:
    bag =
      new excess::concurrent_bag_NBLStack<void>
            (excess::concurrent_bag_NBLStack<void>::LF_B,
             NR_OPERS*NBLExpProducerConsumerBase::MAX_CPUS);
    break;
  case 10:
    bag =
      new excess::concurrent_bag_NBLStack<void>
            (excess::concurrent_bag_NBLStack<void>::LF_ELIM,
             NR_OPERS*NBLExpProducerConsumerBase::MAX_CPUS);
    break;
  case 11:
    bag =
      new excess::concurrent_bag_NBLBag<void>
            (excess::concurrent_bag_NBLBag<void>::LF_BB,
             NR_OPERS*NBLExpProducerConsumerBase::MAX_CPUS);
    break;
  case 12:
    bag =
      new excess::concurrent_bag_NBLBag<void>
            (excess::concurrent_bag_NBLBag<void>::LF_EDTREE,
             NR_OPERS*NBLExpProducerConsumerBase::MAX_CPUS);
    break;
#else
  case 0:
  case 1:
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
  case 9:
  case 10:
  case 11:
  case 12:
    std::cerr << "Error: Compiled without NOBLE support!" << std::endl;
    exit(-1);
    break;
#endif
  }
  // Set up the collection operations pointers.
  Insert = BagInsert;
  TryRemove = BagTryRemove;
}

handle_t* NBLExpProducerConsumerBase::ThreadInitImplementationNr(int nr)
{
  return bag->get_handle();
}

void NBLExpProducerConsumerBase::DeInitImplementationNr(int nr)
{
  delete bag;
  bag = 0;
}

string NBLExpProducerConsumerBase::GetStatistics()
{
  std::stringstream ss;
  ss << countInsert << "  "
     << countOkTryRemove << "  "
     << countEmptyTryRemove;
  return ss.str();
}

string NBLExpProducerConsumerBase::GetStatisticsLegend()
{
  std::stringstream ss;
  ss << "<#Insert operations>  "
     << "<#Non-empty TryRemove operations>  "
     << "<#Empty TryRemove operations>";
  return ss.str();
}

void NBLExpProducerConsumerBase::SaveThreadStatistics(long countInsert,
                                                      long countOkTryRemove,
                                                      long countEmptyTryRemove)
{
  // Update the global operation counters.
  STDFAA(&this->countInsert, countInsert);
  STDFAA(&this->countOkTryRemove, countOkTryRemove);
  STDFAA(&this->countEmptyTryRemove, countEmptyTryRemove);
}
