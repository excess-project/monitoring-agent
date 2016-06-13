#!/bin/bash

GRS_DIR=../../../grs_0.09_snap
BHSPARSE_DIR=$HOME/build/Benchmark_SpGEMM_using_CSR/SpGEMM_cuda

g++ -c -g -fopenmp -DUSE_OPENMP {$GRS_DIR/grs,SpMMTest}.cpp -I $CUDA_DIR/include/ -I ../SpBLAS -I ../include -I $GRS_DIR -I .  -Wno-write-strings && \
g++ -c -g -fopenmp -DUSE_OPENMP {SpGEMM_component,SpGEMM_variant_sequential,SpGEMM_variant_DSParallel}.cpp ../SpBLAS/{SpMatrix,SparseAccumulator}.cpp -I $CUDA_DIR/include/ -I ../SpBLAS -I ../include -I $GRS_DIR -I .  -Wno-write-strings && \
nvcc -O3 -m64 -c -g SpGEMM_variant_bhSPARSE_cuda.cpp $GRS_DIR/help_cuda.cu $BHSPARSE_DIR/{bhsparse.cpp,bhsparse_cuda.cu} -I $CUDA_DIR/include/ -I ../SpBLAS -I ../include -I $GRS_DIR -I $BHSPARSE_DIR -I $CUDA_DIR/samples/common/inc -I . && \
nvcc -g -o grs_spmmtest {SpMMTest,grs,help_cuda,SpGEMM_component,SpGEMM_variant_sequential,SpGEMM_variant_DSParallel,SpGEMM_variant_bhSPARSE_cuda,SpMatrix,SparseAccumulator,bhsparse,bhsparse_cuda}.o -Xcompiler -fopenmp

