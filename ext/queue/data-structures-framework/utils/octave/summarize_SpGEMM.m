%% Summarize the result of a collection of SpGEMM testbench cases.
%% Anders Gidenstam  2016

FREQs = [3];
%ALGs = [3];
ALGs = 0:12;
THREADs = [1 2 4 6 8 10 12 14 16 18 20];
%THREADs = [10];
MMALGs = [0 1];
MATRICEs = 0:13;
%MATRICEs = [5];
%MATRICEs = [0 1 2];
%MATRICEs = [2];
WUSIZEs = [1 2 4 8 16];

plotpower=0;

%% Algorithms: Gustavson; Threads: 1; Pinning: 1; MMAlg: 0; Matrix: 0 1 2; WUSize: -;
%% calloc used in phase 2.
RUNs1 = [
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_14.14'
         ];

%% Algorithms: 0 1 2 3 4 5 6 7 8 9 10 11; Threads: 2 4 6 8 10 12 14 16 18 20; Pinning: 1; MMAlg: 1; Matrix: 1 2; WUSize: 4;
%% calloc used during phase 1 and phase 2.
RUNs2 = [
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_14.32'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_14.35'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_14.38'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_14.41'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_14.44'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_14.48'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_14.51'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_14.54'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_14.57'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_15.00'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_15.04'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_15.07'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_15.10'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_15.35'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_15.38'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_15.41'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_15.44'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_15.47'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_15.51'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_15.54'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_15.57'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_16.00'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_16.03'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_16.07'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_16.10'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_16.13'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_16.16'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_16.19'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_16.22'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_16.26'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_16.29'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_16.32'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_16.35'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_16.38'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_16.42'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_16.45'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_16.48'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_16.51'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_16.54'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_16.57'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_17.01'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_17.04'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_17.07'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_17.10'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_17.13'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_17.17'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_17.20'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_17.23'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_17.26'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_17.29'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_17.32'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_17.36'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_17.39'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_17.42'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_17.45'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_17.48'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_17.52'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_17.55'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_17.58'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_18.01'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_18.04'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_18.07'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_18.11'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_18.14'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_18.17'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_18.20'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_18.23'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_18.27'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_18.30'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_18.33'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_18.36'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_18.39'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_18.42'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_18.46'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_18.49'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_18.52'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_18.55'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_18.58'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_19.02'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_19.05'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_19.08'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_19.11'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_19.14'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_19.17'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_19.21'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_19.24'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_19.27'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_19.30'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_19.33'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_19.37'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_19.40'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_19.43'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_19.46'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_19.49'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_19.52'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_19.56'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_19.59'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_20.02'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_20.05'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_20.08'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_20.11'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_20.15'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_20.18'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_20.21'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_20.24'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_20.27'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_20.31'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_20.34'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_20.37'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_20.40'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_20.43'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_20.46'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_20.50'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_20.53'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_20.56'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_20.59'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_21.02'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_21.06'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_21.09'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_21.12'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_21.15'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_21.18'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_21.21'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_21.25'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_21.28'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_21.31'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_21.34'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_21.37'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_21.41'
'/home/andersg/HLRS/results/SpGEMM_2016-02-04_21.44'
        ];

%% Algorithms: 0 1 2 3 4 5 6 7 8 9 10 11 12; Threads: 20; Pinning: 1; MMAlg: 1; Matrix: 2; WUSize: 2 4 8 16;
%% malloc used during phase 1 and phase 2; mutex + condition used for phase 2.
RUNs3 = [
'/home/andersg/HLRS/results/SpGEMM_2016-02-05_14.22'
         ];

%% Algorithms: 0 1 2 3 4 5 6 7 8 9 10 11 12; Threads: 2 4 6 8 10 12 14 16 18 20; Pinning: 1; MMAlg: 1; Matrix: 2; WUSize: 2 4 8 16;
%% malloc used during phase 1 and phase 2; busy waiting used for phase 2.
RUNs4 = [
'/home/andersg/HLRS/results/SpGEMM_2016-02-05_16.17'
'/home/andersg/HLRS/results/SpGEMM_2016-02-05_17.40'
'/home/andersg/HLRS/results/SpGEMM_2016-02-05_18.37'
'/home/andersg/HLRS/results/SpGEMM_2016-02-05_19.34'
'/home/andersg/HLRS/results/SpGEMM_2016-02-05_20.31'
'/home/andersg/HLRS/results/SpGEMM_2016-02-05_21.28'
'/home/andersg/HLRS/results/SpGEMM_2016-02-05_22.26'
'/home/andersg/HLRS/results/SpGEMM_2016-02-05_23.23'
'/home/andersg/HLRS/results/SpGEMM_2016-02-06_00.20'
'/home/andersg/HLRS/results/SpGEMM_2016-02-06_01.17'
         ];

%% Algorithms: Gustavson; Threads: 1; Pinning: 1; MMAlg: 0; Matrix: 0 1 2; WUSize: -;
%% malloc used in phase 2.
RUNs5 = [
'/home/andersg/HLRS/results/SpGEMM_2016-02-12_10.53'
         ];

%% Algorithms: 0 1 2 3 4 5 6 7 8 9 10 11 12; Threads: 2 4 6 8 10 12 14 16 18 20; Pinning: 1; MMAlg: 1; Matrix: 2; WUSize: 1 2 4 8 16;
%% malloc used during phase 1 and phase 2; busy waiting used for phase 2.
%% Work units are inserted in the P/C collection rather than rows.
RUNs6 = [
'/home/andersg/HLRS/results/SpGEMM_2016-02-12_14.49'
'/home/andersg/HLRS/results/SpGEMM_2016-02-12_15.31'
'/home/andersg/HLRS/results/SpGEMM_2016-02-12_16.10'
'/home/andersg/HLRS/results/SpGEMM_2016-02-12_16.49'
'/home/andersg/HLRS/results/SpGEMM_2016-02-12_17.28'
'/home/andersg/HLRS/results/SpGEMM_2016-02-12_18.07'
'/home/andersg/HLRS/results/SpGEMM_2016-02-13_00.46'
'/home/andersg/HLRS/results/SpGEMM_2016-02-13_01.25'
'/home/andersg/HLRS/results/SpGEMM_2016-02-13_02.04'
'/home/andersg/HLRS/results/SpGEMM_2016-02-13_02.42'
         ];

%% Algorithms: 0; Threads: 1; Pinning: 1; MMAlg: 1; Matrix: 0 1 2 3 4 5 6; WUSize: 1 2 4 8 16;
%% malloc used during phase 1 and phase 2; busy waiting used for phase 2.
%% Work units are inserted in the P/C collection rather than rows.
%% DEBUG is defined to output parallel work in cycles.
%'/home/andersg/HLRS/results/SpGEMM_2016-02-17_14.16/*'

%% Algorithms: Gustavson; Threads: 1; Pinning: 1; MMAlg: 0; Matrix: 0 1 2 3 4 5 6; WUSize: -;
%% malloc used in phase 2.
RUNs7 = [
'/home/andersg/HLRS/results/SpGEMM_2016-02-17_15.02'
         ];

%% Algorithms: 3; Threads: 1; Pinning: 1; MMAlg: 1; Matrix: 0 1 2 3 4 5 6; WUSize: 1 2 4 8 16;
%% malloc used during phase 1 and phase 2; busy waiting used for phase 2.
%% Work units are inserted in the P/C collection rather than rows.
%% DEBUG is defined to output parallel work in cycles.
%'/home/andersg/HLRS/results/SpGEMM_2016-02-17_15.19/*'

%% Algorithms: 3; Threads: 1; Pinning: 1; MMAlg: 1; Matrix: 2 3 4 5 6 7 8 9 10 11 12 13; WUSize: 1 2 4 8 16;
%% malloc used during phase 1 and phase 2; busy waiting used for phase 2.
%% Work units are inserted in the P/C collection rather than rows.
%% DEBUG is defined to output parallel work in cycles.
%% NOTE: The matrices 4- have been renumbered!
%'/home/andersg/HLRS/results/SpGEMM_2016-02-25_13.50/*'
%'/home/andersg/HLRS/results/SpGEMM_2016-02-25_14.50/*'

%% Algorithms: 0-12; Threads: 10; Pinning: 1; MMAlg: 1;
%% Matrix: 2 3 4 5 6 7 8 9 10 11 12 13; WUSize: 1 2 4 8 16;
%% malloc used during phase 1 and phase 2; busy waiting used for phase 2.
%% Work units are inserted in the P/C collection rather than rows.
%% NOTE: A bug in my SpGEMM implementations forced intermediate results to int. 
RUNs8 = [
'/home/andersg/HLRS/results/SpGEMM_2016-02-25_16.48'
'/home/andersg/HLRS/results/SpGEMM_2016-02-25_17.24'
'/home/andersg/HLRS/results/SpGEMM_2016-02-25_18.00'
'/home/andersg/HLRS/results/SpGEMM_2016-02-25_18.36'
'/home/andersg/HLRS/results/SpGEMM_2016-02-25_19.11'
'/home/andersg/HLRS/results/SpGEMM_2016-02-25_19.47'
'/home/andersg/HLRS/results/SpGEMM_2016-02-25_20.23'
'/home/andersg/HLRS/results/SpGEMM_2016-02-25_20.59'
'/home/andersg/HLRS/results/SpGEMM_2016-02-25_21.35'
'/home/andersg/HLRS/results/SpGEMM_2016-02-25_22.10'
'/home/andersg/HLRS/results/SpGEMM_2016-02-25_22.46'
'/home/andersg/HLRS/results/SpGEMM_2016-02-25_23.22'
'/home/andersg/HLRS/results/SpGEMM_2016-02-25_23.58'
         ];

%% Algorithms: 0-12; Threads: 10 20; Pinning: 1; MMAlg: 1;
%% Matrix: 2 3 4 5 6 7 8 9 10 11 12 13; WUSize: 1 2 4 8 16;
%% malloc used during phase 1 and phase 2; busy waiting used for phase 2.
%% Work units are inserted in the P/C collection rather than rows.
RUNs9 = [
'/home/andersg/HLRS/results/SpGEMM_2016-03-03_22.25'
'/home/andersg/HLRS/results/SpGEMM_2016-03-03_23.00'
'/home/andersg/HLRS/results/SpGEMM_2016-03-03_23.36'
'/home/andersg/HLRS/results/SpGEMM_2016-03-04_00.12'
'/home/andersg/HLRS/results/SpGEMM_2016-03-04_00.48'
'/home/andersg/HLRS/results/SpGEMM_2016-03-04_01.24'
'/home/andersg/HLRS/results/SpGEMM_2016-03-04_01.59'
'/home/andersg/HLRS/results/SpGEMM_2016-03-04_02.35'
'/home/andersg/HLRS/results/SpGEMM_2016-03-04_03.11'
'/home/andersg/HLRS/results/SpGEMM_2016-03-04_03.47'
'/home/andersg/HLRS/results/SpGEMM_2016-03-04_04.22'
'/home/andersg/HLRS/results/SpGEMM_2016-03-04_04.58'
'/home/andersg/HLRS/results/SpGEMM_2016-03-04_05.34'
'/home/andersg/HLRS/results/SpGEMM_2016-03-04_06.10'
'/home/andersg/HLRS/results/SpGEMM_2016-03-04_06.46'
'/home/andersg/HLRS/results/SpGEMM_2016-03-04_07.21'
'/home/andersg/HLRS/results/SpGEMM_2016-03-04_07.57'
'/home/andersg/HLRS/results/SpGEMM_2016-03-04_08.33'
'/home/andersg/HLRS/results/SpGEMM_2016-03-04_09.09'
'/home/andersg/HLRS/results/SpGEMM_2016-03-04_09.45'
'/home/andersg/HLRS/results/SpGEMM_2016-03-04_10.20'
'/home/andersg/HLRS/results/SpGEMM_2016-03-04_10.56'
'/home/andersg/HLRS/results/SpGEMM_2016-03-04_11.32'
'/home/andersg/HLRS/results/SpGEMM_2016-03-04_12.08'
'/home/andersg/HLRS/results/SpGEMM_2016-03-04_12.44'
'/home/andersg/HLRS/results/SpGEMM_2016-03-04_13.19'
         ];

%% Algorithms: 3; Threads: 1; Pinning: 1; MMAlg: 1; Matrix: 2 3 4 5 6 7 8 9 10 11 12 13; WUSize: 1 2 4 8 16;
%% malloc used during phase 1 and phase 2; busy waiting used for phase 2.
%% Work units are inserted in the P/C collection rather than rows.
%% DEBUG is defined to output parallel work in cycles.
%'/home/andersg/HLRS/results/SpGEMM_2016-03-04_15.23/*'
%'/home/andersg/HLRS/results/SpGEMM_2016-03-04_16.24/*'

%% Algorithms: 3; Threads: 1; Pinning: 1; MMAlg: 1; Matrix: 2 3 4 5 6 7 8 9 10 11 12 13; WUSize: 1;
%% malloc used during phase 1 and phase 2; busy waiting used for phase 2.
%% Work units are inserted in the P/C collection rather than rows.
%% DEBUG is defined to output parallel work in cycles.

RUNs = RUNs9;

i = 1;
res = [];
for d = 1:rows(RUNs)
  base = RUNs(d,:);
  basename = [base '/SpGEMM_result_' base(length(base) - 15 : length(base)) '-'];

  for f = FREQs
    for a = ALGs

      algname = sprintf("ca%d", a);

      for matrix = MATRICEs
        for wu = WUSIZEs
          for mmalg = MMALGs
            for t = THREADs
              casename = sprintf("mma%d-m%d-wus%d-f%d-t%d", mmalg, matrix, wu, f, t);

              try
                resfile = sprintf("%sOUT-%s-%s.txt", basename, algname, casename);
                %printf("Trying '%s' ... ", outfile);

                [info, err] = stat(resfile);
                if (err == 0)
                  [alg threads pinning matrix mmalg wus durations operations RAPL_powers RAPL_powers_biased_coef_of_var] = summarize_SpGEMM_case(basename, algname, casename, plotpower);
                  %printf("succeeded\n");

                  res(i,:) = [f alg threads pinning matrix mmalg wus durations operations RAPL_powers RAPL_powers_biased_coef_of_var];
                  i = i+1;
                else
                  %printf("failed\n");
                endif
              catch
                printf("exception in summarize_SpGEMM case '%s'\n", resfile);
              end_try_catch
            endfor
          endfor
        endfor
      endfor
    endfor
  endfor
endfor

%% Layout of result file:
%%   1-7:  freq collection_alg threads pinning matrix mmalg wus
%%   8-11: total_duration phase1_duration phase2_duration phase3_duration
%%  12-14: #inserts  #non-empty_TryRemoves #empty_TryRemoves
%%  15-18: P_PKG_S1       P_PKG_S2       P_CPU_S1    P_CPU_S2
%%  19-22: P_Uncore_S1    P_Uncore_S2    P_Mem_S1    P_Mem_S2
%%  23-26: P_PKG_S1_p1    P_PKG_S2_p1    P_CPU_S1_p1 P_CPU_S2_p1
%%  27-30: P_Uncore_S1_p1 P_Uncore_S2_p1 P_Mem_S1_p1 P_Mem_S2_p1
%%  31-34: P_PKG_S1_p2    P_PKG_S2_p2    P_CPU_S1_p2 P_CPU_S2_p2
%%  35-38: P_Uncore_S1_p2 P_Uncore_S2_p2 P_Mem_S1_p2 P_Mem_S2_p2
%%  39-42: P_PKG_S1_p3    P_PKG_S2_p3    P_CPU_S1_p3 P_CPU_S2_p3
%%  43-46: P_Uncore_S1_p3 P_Uncore_S2_p3 P_Mem_S1_p3 P_Mem_S2_p3
%%
%%  47-50: BCoV_PKG_S1       BCoV_PKG_S2       BCoV_CPU_S1    BCoV_CPU_S2
%%  51-54: BCoV_Uncore_S1    BCoV_Uncore_S2    BCoV_Mem_S1    BCoV_Mem_S2
%%  55-58: BCoV_PKG_S1_p1    BCoV_PKG_S2_p1    BCoV_CPU_S1_p1 BCoV_CPU_S2_p1
%%  59-62: BCoV_Uncore_S1_p1 BCoV_Uncore_S2_p1 BCoV_Mem_S1_p1 BCoV_Mem_S2_p1
%%  63-66: BCoV_PKG_S1_p2    BCoV_PKG_S2_p2    BCoV_CPU_S1_p2 BCoV_CPU_S2_p2
%%  67-70: BCoV_Uncore_S1_p2 BCoV_Uncore_S2_p2 BCoV_Mem_S1_p2 BCoV_Mem_S2_p2
%%  71-74: BCoV_PKG_S1_p3    BCoV_PKG_S2_p3    BCoV_CPU_S1_p3 BCoV_CPU_S2_p3
%%  75-78: BCoV_Uncore_S1_p3 BCoV_Uncore_S2_p3 BCoV_Mem_S1_p3 BCoV_Mem_S2_p3

csvwrite("result.res", res);
