// Producer-Consumer microbenchmark for the experiment framework.
// Copyright (C) 2014 - 2016  Anders Gidenstam
// Based on NBLExpQueue.cpp
// Copyright (C) 2011  Håkan Sundell
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

#include "NBLExpProducerConsumer.h"

#include "primitives.h"

#include <sstream>
#include <iomanip>
#include <limits>
#include <cstdlib>
#include <ctime>

//using namespace tbb;

#define MAX_OPS 1048576
#define MAX_PHASES 16

#define _mm_pause()  asm volatile ("rep; nop" : : )
#define PAUSE _mm_pause()
#define PAUSE_BUNCH PAUSE;PAUSE;PAUSE;PAUSE;PAUSE;PAUSE;PAUSE;PAUSE;PAUSE;PAUSE
#define BIG_PAUSE_BUNCH PAUSE_BUNCH;PAUSE_BUNCH;PAUSE_BUNCH;PAUSE_BUNCH;PAUSE_BUNCH;PAUSE_BUNCH;PAUSE_BUNCH;PAUSE_BUNCH;PAUSE_BUNCH

#define producer_parallel_work()\
  do {                                          \
    for(int j=0;j<PRODUCER_PW;j++){             \
      PAUSE_BUNCH;                              \
    }                                           \
  } while(0)

#define consumer_parallel_work()\
  do {                                          \
    for(int j=0;j<CONSUMER_PW;j++){             \
      PAUSE_BUNCH;                              \
    }                                           \
  } while(0)

extern volatile int mainCounter;

typedef struct {
  int op;
  int arg1;
} ListOperation;
 
static volatile ListOperation operationList[NBLExpProducerConsumer::MAX_CPUS][MAX_OPS];

static void RandomizeOperations(int NR_CPUS, int NR_OPERS);

void RandomizeOperations(int NR_CPUS, int NR_OPERS)
{
  int id; 
  for(id=0;id<NR_CPUS;id++) {
    int nr1=0;
    int nr2=0;
    int nr3=0;
    int i; 
    for(i=0;i<NR_OPERS&&i<MAX_OPS;i++) {
      int ok=0;
      int op;
      int key;
      while(!ok) {
        int r=rand()%100;
        op=0;
        if(r>=50)
          op=1;
        key=(rand()%10000)*NR_CPUS+id;
        if(op==0) { /* Insert */
          ok=1;
        } else if(op==1) { /* TryRemove */
          ok=1;
        }
      }
      operationList[id][i].op=op;
      operationList[id][i].arg1=key;
    }
    /*      printf("%d - %d %d %d\n",id,nr1,nr2,nr3); */
  }
}

NBLExpProducerConsumer::NBLExpProducerConsumer(void)
{
  COMM_NR=1;
  PRODUCER_PW=1;
  CONSUMER_PW=1;
  MAX_INSERTS=0;
  phases=0;
  active=0;
  active_duration=0;
  no_phases=0;
}

NBLExpProducerConsumer::~NBLExpProducerConsumer(void)
{
  delete[] phases;
}

string NBLExpProducerConsumer::GetExperimentName()
{
  return string("ProducerConsumer");
}

string NBLExpProducerConsumer::GetCaseName()
{
  std::stringstream ss;
  ss << "-com.pat." << COMM_NR
     << "-Ppw" << PRODUCER_PW
     << "-Cpw" << CONSUMER_PW
     << "-maxins." << MAX_INSERTS;
  return ss.str();
}

vector<string> NBLExpProducerConsumer::GetParameters()
{
  vector<string> v;
  v.push_back(string("Communication pattern"));
  v.push_back(string("Producer parallel work size"));
  v.push_back(string("Consumer parallel work size"));
  v.push_back(string("Restrict collection size, count only active phases"));
  return v;
}

// This is rather ugly.
#define MAX_SINGLEOPS 20
#define COMPAT2PMOD(pat) ((pat)-4)
#define COMPAT2CMOD(pat) ((pat)-3-MAX_SINGLEOPS)

vector<string> NBLExpProducerConsumer::GetParameterValues(int pno)
{
  vector<string> v;
  switch (pno) {
  case 0:
    v.push_back(string("0. N threads randomly selecting between Produce/Consume"));
    v.push_back(string("1. N/2 Producer - N/2 Consumer threads (default)"));
    v.push_back(string("2.   1 Producer - N-1 Consumer threads"));
    v.push_back(string("3. N-1 Producer -   1 Consumer threads"));
    v.push_back(string("4.   0 Producer -   N Consumer threads "
                       "(use phased mode for non-empty)"));
    v.push_back(string("5.   N Producer -   0 Consumer threads"));
    for (int i=6; COMPAT2CMOD(i) <= MAX_SINGLEOPS; i++) {
      std::stringstream ss;
      if (1 < COMPAT2CMOD(i)) {
        ss << i
           << ". Enqueuers are those where do_enq = !(myId % "
           << COMPAT2CMOD(i) << ") is true.";
      } else if (1 < COMPAT2PMOD(i)) {
        ss << i
           << ". Enqueuers are those where do_enq = myId % "
           << COMPAT2PMOD(i) << " is true.";
      }
      v.push_back(ss.str());
    }
    break;
  case 1:
    v.push_back(string("#parallel work units (integer >=0, default 1)"));
    break;
  case 2:
    v.push_back(string("#parallel work units (integer >=0, default 1)"));
    break;
  case 3:
    v.push_back(string("#inserts per producer and active phase (/ "
                       "#pre-filled elements with pattern 4) "
                       "(default 0/disabled)"));
  }
  return v;
}

int NBLExpProducerConsumer::GetParameter(int pno)
{
  switch (pno) {
  case 0:
    return COMM_NR;
  case 1:
    return PRODUCER_PW;
  case 2:
    return CONSUMER_PW;
  case 3:
    return MAX_INSERTS;
  default:
    return -1;
  }
}

void NBLExpProducerConsumer::SetParameter(int pno, int value)
{
  switch (pno) {
  case 0:
    COMM_NR=value;
    break;
  case 1:
    PRODUCER_PW=value;
    break;
  case 2:
    CONSUMER_PW=value;
    break;
  case 3:
    MAX_INSERTS=value;
    if (MAX_INSERTS > 0) {
      delete[] phases;
      phases = new long double[MAX_PHASES];
    }
    break;
  default:
    break;
  }
}

void NBLExpProducerConsumer::CreateScenario()
{
  srand((unsigned int)time(NULL));
  RandomizeOperations(NR_CPUS, NR_OPERS);
}

void NBLExpProducerConsumer::RunImplementationNr(int nr, int threadID)
{
  long countInsert = 0;
  long countOkTryRemove = 0;
  long countEmptyTryRemove = 0;

  handle_t* handle = ThreadInitImplementationNr(nr);

  int myId=(int)threadID;
  int i=0;
  if (!MAX_INSERTS || !COMM_NR) {
    switch (COMM_NR) {
    case 0: // N Randomized Producers/Consumers
      for(;mainCounter;i++) {
        int op    = operationList[myId][i%MAX_OPS].op;
        int* arg1 = const_cast<int*>(&operationList[myId][i%MAX_OPS].arg1);
        
        if(op==0) {
          // Producer parallel work.
          producer_parallel_work();
          
          Insert(handle,(void*)arg1, countInsert);
        } else if(op==1) {
          void* result=
            TryRemove(handle,
                      countOkTryRemove, countEmptyTryRemove);
          // Consumer parallel work.
          consumer_parallel_work();
        }
        i++;
      }
      break;

    case 1: // N/2:N/2 Producers - Consumers
    case 2: // 1:N-1 Producer - Consumers
    case 3: // N-1:1 Producers - Consumer
    case 4: //  0:N  Producers - Consumers
    case 5: //  N:0  Producers - Consumers      
    default:

      // Code common to cases 1- (fixed producer/consumer assignment):
      int do_enq;
      switch (COMM_NR) {
      case 1: do_enq = !(myId % 2); break;
      case 2: do_enq = (myId==0);   break;
      case 3: do_enq = (myId>=1);   break;
      case 4: do_enq = 0;           break;
      case 5: do_enq = 1;           break;
      default:
        if (1 < COMPAT2CMOD(COMM_NR)) {
          do_enq = !(myId % COMPAT2CMOD(COMM_NR));
        } else if (1 < COMPAT2PMOD(COMM_NR)) {
          do_enq = (myId % COMPAT2PMOD(COMM_NR));          
        }
        break;        
      }
      for(;mainCounter;) {
        int* arg1 = const_cast<int*>(&operationList[myId][0].arg1);
        
        if(do_enq) {
          // Producer parallel work.
          producer_parallel_work();

          Insert(handle,(void*)arg1, countInsert);
        } else {
          void* result =
            TryRemove(handle,
                      countOkTryRemove, countEmptyTryRemove);

          // Consumer parallel work.
          consumer_parallel_work();
        }
      }
      break;
    }
  } else {
    // Run the experiment in short phases to handle large differences
    // in producer/consumer rates.
    // This only makes sense for fixed producer/consumer assignments.
    long max_inserted = MAX_INSERTS;
    // The leader thread controls the phase switches.
    int leader        = 0;
    int phase_shift   = 0;
    int phase         = 0;
    long ins_by_me    = 0;
    long double time_active = 0;
    struct timespec now;
    long dummy1=0, dummy2=0;

    // Code common to cases 1- (fixed producer/consumer assignment):
    int do_enq;
    switch (COMM_NR) {
    case 1: do_enq = !(myId % 2); leader=!myId;     break;
    case 2: do_enq = (myId==0);   leader=!myId;     break;
    case 3: do_enq = (myId>=1);   leader=(myId==1); break;
    case 4: do_enq = 0;           leader=!myId;     break;
    case 5: do_enq = 1;           leader=!myId;     break;
    default:
      if (1 < COMPAT2CMOD(COMM_NR)) {
        do_enq = !(myId % COMPAT2CMOD(COMM_NR));
        leader=!myId;
      } else if (1 < COMPAT2PMOD(COMM_NR)) {
        do_enq = (myId % COMPAT2PMOD(COMM_NR));          
        leader=(myId==1);
      }
      break;        
    }
    if (leader) {
      clock_gettime(CLOCK_REALTIME, &now);
      phases[phase] = now.tv_sec + 1e-9*now.tv_nsec;
      active=(COMM_NR!=4); // The 0 producers case starts inactive.
    }

    for(;mainCounter;) {
      int* arg1 = const_cast<int*>(&operationList[myId][0].arg1);

      if(do_enq) { // Not used in the 0 producers case.
        // Producer.
        producer_parallel_work();
        if (active) {
          Insert(handle,(void*)arg1, countInsert);
          ins_by_me++;
          if (leader && (ins_by_me==max_inserted)) {
            active = 0;
            ins_by_me = 0;
            phase_shift = 1;
          }
        } else {
          // Drain the collection.
          void* result = TryRemove(handle, dummy1, dummy2);
          if (!result && leader) {
            ins_by_me++;
            if (ins_by_me==20) {
              ins_by_me = 0;
              active = 1;
              phase_shift = 1;
            }
          } else {
            ins_by_me = 0;
          }
        }
      } else {
        // Consumer.
        if (active) {
          void* result =
            TryRemove(handle,
                      countOkTryRemove, countEmptyTryRemove);
          if (leader && !result) {
            // The 0 producers case: the collection has been emptied.
            active = 0;
            ins_by_me = 0;
            phase_shift = 1;
          }
        } else {
          if (COMM_NR != 4) {
            void* result =
              TryRemove(handle, dummy1, dummy2);
          } else if (leader) {
            // The 0 producers case: pre-fill the collection for the next phase.
            Insert(handle,(void*)arg1, dummy1);
            ins_by_me++;
            if (leader && (ins_by_me==max_inserted)) {
              active = 1;
              phase_shift = 1;
            }
          }
        }
        consumer_parallel_work();
      }
      if (phase_shift) {
        int start = phase % MAX_PHASES;
        int end   = (++phase) % MAX_PHASES;

        phase_shift = 0;
        clock_gettime(CLOCK_REALTIME, &now);
        phases[end] = now.tv_sec + 1e-9*now.tv_nsec;
        if (!active) {
          time_active += (phases[end] - phases[start]);
        }
      }
    }
    if (leader) {
      // Count the final unfinished phase IFF it is an active phase.
      if (active) {
        int start = phase % MAX_PHASES;
        int end   = (++phase) % MAX_PHASES;
        clock_gettime(CLOCK_REALTIME, &now);
        phases[end] = now.tv_sec + 1e-9*now.tv_nsec;
        time_active += (phases[end] - phases[start]);
      }
      // Store the active time and number of phases.
      active_duration = time_active;
      no_phases = phase;
    }
  }
  // Update the global operation counters from all/active phases.
  SaveThreadStatistics(countInsert,
                       countOkTryRemove, countEmptyTryRemove);
  delete handle;
}

string NBLExpProducerConsumer::GetStatistics()
{
  std::stringstream ss;
  ss << NBLExpProducerConsumerBase::GetStatistics()
     << " " << COMM_NR
     << " " << PRODUCER_PW
     << " " << CONSUMER_PW;
  if (MAX_INSERTS && COMM_NR) {
    ss <<  std::setprecision(std::numeric_limits<long double>::digits10)
       << " " << MAX_INSERTS
       << " " << active_duration
       << " " << no_phases;
    int i = (no_phases < MAX_PHASES) ? 0 : no_phases - (MAX_PHASES-1);
    for (; i <= no_phases; i++) {
      ss << " " << phases[i % MAX_PHASES];
    }
  }
  return ss.str();
}

string NBLExpProducerConsumer::GetStatisticsLegend()
{
  std::stringstream ss;
  ss << NBLExpProducerConsumerBase::GetStatisticsLegend()
     << " <communication pattern#>"
     << " <producer parallel work>"
     << " <consumer parallel work>";
  if (MAX_INSERTS && COMM_NR) {
    ss << " <maximum collection size>"
       << " <active duration>"
       << " <#phases>"
       << " <start/end of the last "
       << (MAX_PHASES-1)
       <<  " active/passive phases>";
  }
  return ss.str();
}
