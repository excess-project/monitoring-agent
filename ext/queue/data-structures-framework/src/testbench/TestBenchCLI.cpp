// CLI interface to the NOBLE/EXCESS data structures test framework
// Copyright (C) 2011 - 2016  Anders Gidenstam
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

#include <iostream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <pthread.h>
#include <unistd.h>
#include <sched.h>

#include "NBLExpProducerConsumer.h"
#include "NBLExpMandelbrot.h"
#include "NBLExpGEMM.h"
#include "NBLExpSpGEMM.h"
#include "NBLExpDictionary.h"
#include "NBLExpWeakDictionary.h"

#include "primitives.h"

#ifdef USE_EXCESS_MF
extern "C" {
#include <mf_api.h>
}
static int useMF = 0;
#endif

// Machine configuration for pinning.
// NOTE: The enumeration order of PUs may differ by
//       architecture and kernel version.
//       Verify that the order assumed in pin_thread()
//       below matches that of your machine.
const int cores_per_socket = 8;
const int PUs_per_socket = 8;
const int sockets = 2;

typedef enum {
  NONE=-1, FILL_PU, FILL_CORE, JUMP_SOCKET
} pinning_strategy_t;

pinning_strategy_t pinning = NONE; // The default is None.

static NBLExperiment* experiment = 0;
static int            experimentNr = 0;
static vector<int>    active_implementations = vector<int>();
static int            current_implementation = -1;
static int            nrThreads = 1;
static useconds_t     expDuration = 0;

static volatile int runningCounter=0;
static volatile int go = 0;
static struct   timespec init_time, start_time, end_time;

// Stop signal for the worker threads.
volatile int mainCounter=1;

int*         cacheMemory=0;

static void perform_experiment();
static void process_arguments(int argc, char** argv);
static void print_usage(int argc, char** argv);
static void print_output_legend();
static bool try_parse(string s, int& i);
static void clean_caches();
static void* experiment_thread(void* arg);
static void* clean_thread(void* arg);
static void pin_thread(int tid, pinning_strategy_t strategy);

int main(int argc, char** argv)
{
  experiment = new NBLExpProducerConsumer();
  process_arguments(argc, argv);

#ifdef USE_EXCESS_MF
  if (useMF) {
    // Prepare the ATOM MF API.
    //   Environment when using PBS on the EXCESS cluster:
    //     USER        : workflow ID
    //     PBS_JOBID   : PBS job ID (not used by MF)
    //     PBS_JOBNAME : task ID
    //     MF_DBKEY    : MF experiment ID (not set by MF!?)
    char* wf_id   = getenv("USER");
    char* exp_id  = getenv("MF_DBKEY");
    char* task_id = getenv("PBS_JOBNAME");
    mf_api_initialize(EXCESSMFURL, wf_id, exp_id, task_id);
  }
#endif

  perform_experiment();

  delete experiment;

#ifdef USE_EXCESS_MF
  if (useMF) {
    // Wait some, hoping the MF will finish sending its data.
    usleep(10000000);
  }
#endif

  return 0;
}

static void perform_experiment()
{
  experiment->SetNrThreads(nrThreads);
  experiment->CreateScenario();

  for (int impl = 0; impl < active_implementations.size(); impl++) {
    pthread_t thread[nrThreads];

    // Measure how much time is spent before GO.
    clock_gettime(CLOCK_REALTIME, &init_time);

    current_implementation = active_implementations[impl];
    //std::cout << "Preparing " <<
    //  experiment->GetImplementations()[current_implementation] << "...";

#ifdef USE_EXCESS_MF
    // Prepare experiment case name.
    std::stringstream ss;
    ss << experiment->GetExperimentName()
       << "-i" << current_implementation
       << "-t" << nrThreads
       << "-p" << pinning
       << experiment->GetCaseName();

    string casename = ss.str();
#endif

    experiment->InitImplementationNr(current_implementation);

    // PERFORM EXPERIMENT
    //   Do not run clean_caches. The difference should be very small on a
    //   multi-second experiment.
    //clean_caches();
    runningCounter=0;
    go=0;
    mainCounter=1;

    for(int i=0;i<nrThreads;i++)
      pthread_create(&thread[i], 0, experiment_thread, (void*)(long)i);
    //std::cout << "Waiting for the threads to start...";
    //std::cout.flush();
    while (runningCounter < nrThreads) {
      usleep(100);
    }
    //std::cout << "starting...";

#ifdef USE_EXCESS_MF
    if (useMF) {
      // Send start of case signal to the MF.
      mf_api_start_profiling(casename.c_str());
    }
#endif

    clock_gettime(CLOCK_REALTIME, &start_time);
    go=1;
    //std::cout << "started...";
    //std::cout.flush();

    usleep(expDuration);
    mainCounter = 0; // Tell the threads to stop.
    //std::cout << "stopping...";

    for (int i=0;i<nrThreads; i++)
      pthread_join(thread[i], 0);
    //std::cout << "done." << std::endl;

#ifdef USE_EXCESS_MF
    if (useMF) {
      // Send end of case signal to the MF.
      mf_api_stop_profiling(casename.c_str());
    }
#endif

    double duration =
      (double)(end_time.tv_sec - start_time.tv_sec) +
      1e-9 * (double)(end_time.tv_nsec - start_time.tv_nsec);
    double initDelay =
      (double)(start_time.tv_sec - init_time.tv_sec) +
      1e-9 * (double)(start_time.tv_nsec - init_time.tv_nsec);

    // Note: Update the legend too when changing anything here.
    std::cout <<
      std::setprecision(std::numeric_limits<long double>::digits10) <<
      current_implementation <<
      "  " << nrThreads <<
      "  " << pinning <<
      "  " << duration <<
      "  " << initDelay <<
      "  " << ((long double)start_time.tv_sec +
	       1e-9 * (long double)start_time.tv_nsec) <<
      "  " << experiment->GetStatistics() << std::endl;

    experiment->DeInitImplementationNr(current_implementation);
  }
}

static void process_arguments(int argc, char** argv)
{
  int i = 1;
  while (i < argc) {
    string arg = string(argv[i]);
    if (arg.compare("-h") == 0) {
      print_usage(argc, argv);
      exit(0);
    } else if (arg.compare("-e") == 0) {
      int e;
      if ((++i < argc) && try_parse(string(argv[i]), e)) {
        experimentNr = e;
        if (e != 0) {
          delete experiment;
        }
        switch (e) {
        case 0:
          // Keep the producer-consumer experiment.
          break;
        case 1:
          experiment = new NBLExpApplicationMandelbrot();
          break;
#ifdef USE_BLAS
        case 2:
          experiment = new NBLExpGEMM();
          break;
#endif
        case 3:
          experiment = new NBLExpSpGEMM();
          break;
        case 4:
          experiment = new NBLExpDictionary();
          break;
        case 5:
          experiment = new NBLExpWeakDictionary();
          break;
        default:
          std::cerr << "Error: Bad experiment# given with -e." << std::endl;
          print_usage(argc, argv);
          exit(-1);
        }
      }
    } else if (arg.compare("-t") == 0) {
      int t;
      if ((++i < argc) && try_parse(string(argv[i]), t) && (0 < t)) {
        nrThreads = t;
      } else {
        std::cerr << "Error: Bad #thread given with -t." << std::endl;
        print_usage(argc, argv);
        exit(-1);
      }
    } else if (arg.compare("-i") == 0) {
      int t;
      if ((++i < argc) && try_parse(string(argv[i]), t) &&
          (0 <= t) && (t < experiment->GetImplementations().size())) {
        active_implementations.push_back(t);
      } else {
        std::cerr << "Error: Bad implementation# given with -i." << std::endl;
        print_usage(argc, argv);
        exit(-1);
      }
    } else if (arg.compare("-d") == 0) {
      int seconds;
      if ((++i < argc) && try_parse(string(argv[i]), seconds) &&
          (0 < seconds)) {
        expDuration = (useconds_t)seconds * 1000000;
      } else {
        std::cerr << "Error: Bad experiment duration given with -d."
                  << std::endl;
        print_usage(argc, argv);
        exit(-1);
      }
    } else if (arg.compare("-p") == 0) {
      int strategy;
      if ((++i < argc) && try_parse(string(argv[i]), strategy) &&
          (NONE <= strategy) && (strategy <= JUMP_SOCKET)) {
        pinning = (pinning_strategy_t)strategy;
      } else {
        std::cerr << "Error: Bad pinning strategy given with -p." << std::endl;
        print_usage(argc, argv);
        exit(-1);
      }
    } else if (arg.compare("-l") == 0) {
      print_output_legend();
      exit(0);
    } else if (arg.compare("-s") == 0) {
      int pno, value;
      int ok = 0;
      if ((++i < argc) && try_parse(string(argv[i]), pno) &&
          (0 <= pno) && (pno < experiment->GetParameters().size())) {
        if ((++i < argc) && try_parse(string(argv[i]), value) &&
            (0 <= value) &&
            ((experiment->GetParameterValues(pno).size() <= 1) ||
             value < experiment->GetParameterValues(pno).size())) {
          experiment->SetParameter(pno, value);
          ok = 1;
        } else {
          std::cerr << "Error: Bad value given with -s for parameter " << pno
                    << "(" << experiment->GetParameters()[pno] << ")."
                    << std::endl;
        }
      } else {
        std::cerr << "Error: Bad parameter# given with -s." << std::endl;
      }
      if (!ok) {
        print_usage(argc, argv);
        exit(-1);
      }
#ifdef USE_EXCESS_MF
    } else if (arg.compare("-mf") == 0) {
      useMF = 1;
#endif
    } else {
      std::cerr << "Error: Unknown argument '" << argv[i] << "'." << std::endl;
      print_usage(argc, argv);
      exit(-1);
    }
    i++;
  }
}

static void print_usage(int argc, char** argv)
{
  using std::cout;
  using std::endl;

  cout << endl;
  cout << "EXCESS data structures experiment framework." << endl;
  //cout << "  Copyright (C) 2011 - 2015  Anders Gidenstam" << endl;
  //cout << "  Copyright (C) 2011         Håkan Sundell" << endl;

  cout << endl;
  cout << "Usage: " << argv[0] << " [options] " << endl;
  cout << endl;

  cout << "  -h                Print this message and exit." << endl;
  cout << "  -e <experiment#>  Set the experiment type." << endl;
  cout << "                    <experiment#> can be one of the following."
       << endl;
  {
    cout << "                      " << "0.  " << "Producer-Consumer microbenchmark."
         << ((experimentNr == 0) ? " (selected)" : "")
         << endl;
    cout << "                      " << "1.  " << "Mandelbrot application."
         << ((experimentNr == 1) ? " (selected)" : "")
         << endl;
#ifdef USE_BLAS
    cout << "                      " << "2.  " << "SGEMM microbenchmark."
         << ((experimentNr == 2) ? " (selected)" : "")
         << endl;
#endif
    cout << "                      " << "3.  " << "SpDGEMM microbenchmark."
         << ((experimentNr == 3) ? " (selected)" : "")
         << endl;
    cout << "                      " << "4.  " << "Dictionary microbenchmark."
         << ((experimentNr == 4) ? " (selected)" : "")
         << endl;
    cout << "                      " << "5.  " << "Weak dictionary microbenchmark."
         << ((experimentNr == 5) ? " (selected)" : "")
         << endl;
  }

  cout << "  -l                Print the output legend for the selected experiment and exit." << endl;
  cout << "  -t <#threads>     Set the number of threads." << endl;
  cout << "  -d <#seconds>     Set the duration of each experiment in seconds."
       << endl;
  cout << "  -p <pinning#>     Set the pinning strategy." << endl;
  cout << "                    The machine type is configured at compile time."
       << endl;
  cout << "                    <pinning#> can be one of the following." << endl;
  {
    cout << "                      " << "-1. " << "NONE. (default)" << endl;
    cout << "                      " << "0.  " << "FILL PU per socket." << endl;
    cout << "                      " << "1.  " << "FILL CORE per socket." << endl;
    cout << "                      " << "2.  " << "JUMP socket." << endl;
  }
  cout << "  -i <impl#>        Include <impl#> in the experiment." << endl;
  cout << "                    For the selected experiment <impl#> can" << endl;
  cout << "                    be one of the following." << endl;
  {
    vector<string> impls = experiment->GetImplementations();
    for (unsigned int i = 0; i < impls.size(); i++) {
      cout << "                      " << i << ". " << impls[i] << endl;
    }
  }
  cout << "  -s <p#> <value>   Set experiment parameter <p#> to <value>."
       << endl;
  cout << "                    For the selected experiment <p#> and <value> can"
       << endl;
  cout << "                    be one of the following." << endl;
  {
    vector<string> parameters = experiment->GetParameters();
    for (unsigned int i = 0; i < parameters.size(); i++) {
      vector<string> values = experiment->GetParameterValues(i);
      if (!values.size()) continue;
      cout << "                      " << i << ". " << parameters[i] << endl;
      for (unsigned int v = 0; v < values.size(); v++) {
        cout << "                          " << values[v] << endl;
      }
    }
  }
#ifdef USE_EXCESS_MF
  cout << "  -mf               Use the EXCESS monitoring framework."
       << endl;
#endif
  cout << endl;
  cout << "Output legend: ";
  print_output_legend();
}

static void print_output_legend()
{
  std::cout <<
    "<implementation#>" <<
    "  <#threads>" <<
    "  <pinning#>" <<
    "  <duration sec>" <<
    "  <initialization delay sec>" <<
    "  <start time instant in seconds>" <<
    "  " << experiment->GetStatisticsLegend() << std::endl;
}

static bool try_parse(string s, int& i)
{
  std::stringstream ss(s);
  char c;
  ss >> i;
  return !(ss.fail() || ss.get(c));
}

static void clean_caches()
{
  const int max_threads = 32;
  pthread_t thread[max_threads];
  int i;

  //std::cout << "Cleaning caches...";
  //std::cout.flush();

  cacheMemory=(int*)malloc(100000096*sizeof(int));

  runningCounter=0;
  go=0;
  for(i=0; i < max_threads; i++)
    pthread_create(&thread[i], 0, clean_thread, (void*)(long)i);

  while (runningCounter < max_threads) {
    usleep(1000);
  }
  go=1;

  for (i=0;i < max_threads; i++)
    pthread_join(thread[i], NULL);

  //std::cout << "done." << std::endl;

  free(cacheMemory);
}

static void* experiment_thread(void* arg)
{
  int i;
  int myId=(int)(long)arg;
  FAA32(&runningCounter, 1);

  /* Set thread affinity. */
  pin_thread(myId, pinning);

  while (!go) ;    /* WAIT FOR START SIGNAL */

  experiment->RunImplementationNr(current_implementation, myId);

  if (FAA32(&runningCounter, -1) == 1) {
    clock_gettime(CLOCK_REALTIME, &end_time);
  }

  return NULL;
}

static void* clean_thread(void* arg)
{
  int i;
  int myId=(int)(long)arg;
  FAA32(&runningCounter, 1);
  while(!go);    /* WAIT FOR START SIGNAL */

  for(i=0;i<100000000;i+=64) {
    cacheMemory[i+myId]=0;
  }

  FAA32(&runningCounter, -1);

  return NULL;
}

// Pin the current thread to a particular PU.
// NOTE: The current code assumes that the all the first PUs of each
//       socket and core is enumerated first, then the second PUs of
//       each core.
static void pin_thread(int tid, pinning_strategy_t strategy)
{
  const int total_cores = sockets * cores_per_socket;
  const int total_PUs = sockets * PUs_per_socket;
  const int PUs_per_core = PUs_per_socket/cores_per_socket;
  int bit = 0;
  cpu_set_t mask;
  CPU_ZERO(&mask);

  switch (strategy) {
  case NONE:
  default:
    //std::cerr << "pinning = NONE" << std::endl;
    return;
  case FILL_PU:
    //std::cerr << "pinning = FILL_PU" << std::endl;
    bit = (tid/PUs_per_core + (tid % PUs_per_core)*total_cores) % total_PUs;
    break;
  case FILL_CORE:
    //std::cerr << "pinning = FILL_CORE" << std::endl;
    bit = tid % total_PUs;
    break;
  case JUMP_SOCKET:
    //std::cerr << "pinning = JUMP_SOCKET" << std::endl;
    bit =
      (tid / sockets + (tid % sockets) * cores_per_socket +
       (tid/total_cores)*cores_per_socket) % total_PUs;
    break;
  }
  CPU_SET(bit % total_PUs, &mask);
  sched_setaffinity(0, sizeof(cpu_set_t), &mask);
}
