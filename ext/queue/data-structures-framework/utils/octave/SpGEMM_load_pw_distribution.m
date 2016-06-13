%% Load parallel work distribution over rows from debug SpGEMM testbench cases.
%% Anders Gidenstam  2016

function pw_distribution = SpGEMM_load_pw_distribution(basename, matrix_no)

  %% MMAlg 1 parallel work measurements.
  %% Keep these constant for parallel work collection.
  calg    = 3;
  mmalg   = 1;
  wus     = 1;
  threads = 1;
  freq    = 3;

  %% Name of the relevant result file.
  pwfile = sprintf("%sOUT-ca%d-mma%d-m%d-wus%d-f%d-t%d.txt",
                   basename, calg, mmalg, matrix_no, wus, freq, threads);
  %% Layout of the parallel work measurements files:
  %%   First line is the standard result output that should be ignored.
  %%   NOTE: For now a leading '%' character MUST be added. 
  %%   Then follows:
  %%   1-4:  row#  cycles_phase1  cycles_phase3  non-zero_count
  pw_distribution = load(pwfile);

endfunction
