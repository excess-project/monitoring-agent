// Minimal sparse double matrix GEMM test for GRS.
// Copyright (C) 2016  Anders Gidenstam
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
#include <ctime>
#include <iostream>
#include <sstream>

#include <grs.h>

#include "SpGEMM_component.h"

static void process_arguments(int argc, char** argv);
static void print_usage(int argc, char** argv);
static bool try_parse(std::string s, int& i);
/*static void print_result(algorithm_t algorithm,
                         SpMatrix& A, SpMatrix& C,
                         double duration);*/
static void test_grs_spmm_csr(int variant);

static std::string matrix_filename;
static std::string result_matrix_filename;
static int         variant = 0;

int main(int argc, char** argv)
{
  process_arguments(argc, argv);

  std::cout << "Testing grs_SpGEMM_csr component..." << std::endl;
  test_grs_spmm_csr(variant);
  std::cout << "done." << std::endl;

  return 0;
}

static void test_grs_spmm_csr(int variant)
{
  struct timespec t1, t2;
  grs_init(2); //Initialize number of modes.

  // Setup GRS execution graph.
  //   Components
  EXCESS_GRS::SpGEMM_component_csr spmm;
  //   Structure
  struct grs_call spmm_call;

  std::cout << "Loading the matrix '" << matrix_filename
            << "' into A ... ";
  EXCESS_GRS::matrix_csr A(SpMatrix::LoadFromFile(matrix_filename));
  std::cout << "Ok." << std::endl;
  std::cout << "A is " << A.rows() << "x" << A.columns() << " matrix with "
            << A.nonzeros() << " non-zeros." << std::endl; 
  EXCESS_GRS::matrix_csr Res(0, 0);

  spmm.setup_call(&spmm_call,
                  "call to SpGEMM_component_csr",
                  A, A, Res);

  //   Regions
  struct grs_region r0_spmm_main;
  grs_region_init(&r0_spmm_main, GRS_SEQ);
  grs_region_set_name(&r0_spmm_main,   "r0_spmm_main");

  struct grs_region r1_spmm_call;
  grs_region_init(&r1_spmm_call, GRS_CALL);
  grs_region_set_name(&r1_spmm_call, "call to SpGEMM_component_csr");
  grs_region_set_calls(&r1_spmm_call, &spmm_call);

  //   Connect regions.
  grs_region_set_seq(&r0_spmm_main, &r1_spmm_call);

  // Select GRS component variants.

  //grs_optimize(&r0_spmm_call);

  //   Variants are currently MANUALLY selected.
  grs_call_set_mode(&spmm_call, variant);

  // Execute the GRS execution graph.
  clock_gettime(CLOCK_REALTIME, &t1);

  //grs_run(&r0_spmm_main);
  grs_run_region_new3(&r1_spmm_call);

  clock_gettime(CLOCK_REALTIME, &t2);

  std::cout << "Execution time: "
            << ((double)(t2.tv_sec - t1.tv_sec) +
                1e-9 * (double)(t2.tv_nsec - t1.tv_nsec))
            << " sec" << std::endl << std::endl;

  std::cout << "Res is " << Res.rows() << "x" << Res.columns()
            << " matrix with " << Res.nonzeros() << " non-zeros." << std::endl; 
  if (!result_matrix_filename.empty()) {
    Res.save_to_file(result_matrix_filename);
  }

  // Cleanup.
  grs_finalize();
}

static void process_arguments(int argc, char** argv)
{
  using std::string;

  int i = 1;
  while (i < argc - 1) {
    string arg = string(argv[i]);
    if (arg.compare("-h") == 0) {
      print_usage(argc, argv);
      exit(0);
    }  else if (arg.compare("-v") == 0) {
      int var;
      if ((++i < argc) && try_parse(string(argv[i]), var) &&
          (0 <= var) && (var < 2)) {
        variant = var;
      } else {
        std::cerr << "Error: Bad variant# given with '-v " << argv[i] << "'."
                  << std::endl;
        print_usage(argc, argv);
        exit(-1);
      }
    } else if (arg.compare("-o") == 0) {
      if (++i < argc - 1) {
        result_matrix_filename = string(argv[i]);
      } else {
        std::cerr << "Error:  No filename given with '-o <file>'."
                  << std::endl;
        print_usage(argc, argv);
        exit(-1);
      }
    } else {
      std::cerr << "Error:  Unrecognized command line argument '"
                << arg << "'." << std::endl;
      print_usage(argc, argv);
      exit(-1);
    }
    i++;
  }
  if (i == argc - 1) {
    matrix_filename = string(argv[i]);
  } else {
    std::cerr << "Error: No matrix file given." << std::endl;
    print_usage(argc, argv);
    exit(-1);
  }
}

static void print_usage(int argc, char** argv)
{
  using std::cout;
  using std::endl;

  cout << endl;
  cout << "EXCESS GRS sparse matrix multiplication experiment." << endl;
  //cout << "  Copyright (C) 2016  Anders Gidenstam" << endl;

  cout << endl;
  cout << "Usage: " << argv[0] << " [options] <matrix file>" << endl;
  cout << endl;

  cout << "  -h                Print this message and exit." << endl;
  cout << "  -v <variant#>     Set the GRS SpGEMM variant to use."
       << endl;
  {
    cout << "                      " << "0.  " << "CPU (New EXCESS algorithm)."
         << endl;
    cout << "                      " << "1.  " << "GPU (bhSPARSE CSR cuda)."
         << endl;
  }
  cout << "  -o <file>       Save the resulting matrix in <file> in MatrixMarket format."
       << endl; 
}

 static bool try_parse(std::string s, int& i)
{
  std::stringstream ss(s);
  char c;
  ss >> i;
  return !(ss.fail() || ss.get(c));
}

/*
static void print_result(algorithm_t algorithm,
                         SpMatrix& A, SpMatrix& C,
                         double duration)
{
  // Condensed result without initial octave comment marker.
  std::cout << algorithm << " "
            << A.m << " " << A.n << " " << A.nzmax << " "
            << duration << " "
            << C.m << " " << C.n << " " << C.nzmax << std::endl;
}
*/
