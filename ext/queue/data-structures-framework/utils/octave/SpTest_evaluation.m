%% Evaluation of SpMM Test testbench cases.
%% Anders Gidenstam  2016

%% EXCESS sparse matrix multiplication experiment.
%%
%% Usage: ../../src/SpBLAS/sptest [options] <matrix file>
%%
%%   -h                Print this message and exit.
%%   -a <algorithm#>   Set the SpGEMM algorithm/library to use.
%%                     <algorithm#> can be one of the following.
%%                       0.  New EXCESS algorithm with row store.
%%                       1.  New EXCESS algorithm with triplet store.
%%                       2.  Sequential Gustavson algorithm.
%%                       3.  librsb.
%%                       4.  CombBLAS.
%%
%% Matrix
%%   0. R14;         1. R15;        2. R16;         3. ASIC_320k;
%%   4. cage12;      5. m133-b3;    6. majorbasis;  7. patents_main;
%%   8. poisson3Da;  9. rajat31;   10. scircuit;   11. sme3Dc;
%%  12.torso1;      13. webbase-1M; (default 0)
%% File format:
%% freq #threads_available matrix# MMalg# A_m A_n A_nnz time res_m res_n res_nnz

%% NOTE: Old file format.
%res=load('/home/andersg/HLRS/results/SpTest_2016-02-25_13.05.res');
%% New file format.
res=load('/home/andersg/HLRS/results/SpTest_2016-03-03_17.25.res');

range_matrices = unique(res(:,3));
range_mmalgs = unique(res(:,4));

res_time = zeros(length(range_matrices),length(range_mmalgs));
for idx_matrix = 1:rows(range_matrices)
  res_time(idx_matrix,:) = res(res(:,3)==range_matrices(idx_matrix),8);
endfor

bar(range_matrices, res_time);
title("SpMM running time for matrix squared.");
legend("New algorithm", "Sequential Gustavson", "librsb");
xlabel("Matrix no.");
ylabel("Time (sec)");

print("SpTest-time.eps", "-deps", "-color");
