%% Summarize the result of a collection of Producer-Consumer testbench cases.
%% Anders Gidenstam  2014 - 2016

FREQs = [3];
%ALGs = [10];
%ALGs = [0 3 7 10];
ALGs = 0:12;
%PPWs = [1 10];
%PPWs = [1 10 150 500 1000];
PPWs = [1 2 3 4 5 7 10 20 50 150 500 1000];
%CPWs = [150 500];
%CPWs = [1 10 150 500 1000];
CPWs = [1 2 3 4 5 7 10 20 50 150 500 1000];
%THREADs = [10];
THREADs = [2 4 6 8 10 12 14 16 18 20];

PLOT_POWER = 0;

%% Algorithms: 0-11; Threads: 2 4 6 8 10 12 14 16 18 20; Pinning 1
RUNs1 = [
'/home/andersg/HLRS/results/producerconsumer_2015-01-30_15.57'
'/home/andersg/HLRS/results/producerconsumer_2015-01-30_16.49'
'/home/andersg/HLRS/results/producerconsumer_2015-01-30_17.42'
'/home/andersg/HLRS/results/producerconsumer_2015-01-30_18.34'
'/home/andersg/HLRS/results/producerconsumer_2015-01-30_19.26'
'/home/andersg/HLRS/results/producerconsumer_2015-01-30_20.19'
'/home/andersg/HLRS/results/producerconsumer_2015-01-30_21.11'
'/home/andersg/HLRS/results/producerconsumer_2015-01-30_22.04'
'/home/andersg/HLRS/results/producerconsumer_2015-01-30_22.56'
'/home/andersg/HLRS/results/producerconsumer_2015-01-30_23.48'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_01.13'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_02.06'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_02.58'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_03.51'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_04.43'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_05.35'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_06.28'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_07.20'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_08.12'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_09.05'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_09.57'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_10.49'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_11.42'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_12.34'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_13.26'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_14.19'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_15.11'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_16.03'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_16.56'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_17.48'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_18.40'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_19.33'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_20.25'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_21.17'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_22.10'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_23.02'
'/home/andersg/HLRS/results/producerconsumer_2015-01-31_23.54'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_01.19'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_02.12'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_03.04'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_03.57'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_04.49'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_05.41'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_06.33'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_07.26'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_08.18'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_09.11'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_10.03'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_10.55'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_11.48'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_12.40'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_13.32'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_14.24'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_15.17'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_16.09'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_17.02'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_17.54'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_18.46'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_19.38'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_20.31'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_21.23'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_22.15'
'/home/andersg/HLRS/results/producerconsumer_2015-02-01_23.08'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_00.00'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_01.25'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_02.18'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_03.10'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_04.02'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_04.55'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_05.47'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_06.39'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_07.32'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_08.24'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_09.16'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_10.09'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_11.01'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_11.53'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_12.46'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_13.38'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_14.31'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_15.23'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_16.15'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_17.08'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_18.00'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_18.52'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_19.45'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_20.37'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_21.29'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_22.22'
'/home/andersg/HLRS/results/producerconsumer_2015-02-02_23.14'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_00.39'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_01.31'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_02.24'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_03.16'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_04.09'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_05.01'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_05.53'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_06.46'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_07.38'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_08.30'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_09.23'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_10.15'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_11.07'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_12.00'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_12.52'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_13.44'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_14.37'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_15.29'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_16.22'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_17.14'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_18.06'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_18.58'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_19.51'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_20.43'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_21.36'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_22.28'
'/home/andersg/HLRS/results/producerconsumer_2015-02-03_23.20'
'/home/andersg/HLRS/results/producerconsumer_2015-02-04_00.46'
'/home/andersg/HLRS/results/producerconsumer_2015-02-04_01.38'
'/home/andersg/HLRS/results/producerconsumer_2015-02-04_02.30'
       ];

%% Algorithms: 0 2 3 6 10; Threads: 2 10 12 20; Pinning: 1
RUNs2 = [
'/home/andersg/HLRS/results/producerconsumer_2015-02-05_15.50'
'/home/andersg/HLRS/results/producerconsumer_2015-02-05_16.42'
'/home/andersg/HLRS/results/producerconsumer_2015-02-05_17.34'
'/home/andersg/HLRS/results/producerconsumer_2015-02-05_18.26'
'/home/andersg/HLRS/results/producerconsumer_2015-02-05_19.19'
'/home/andersg/HLRS/results/producerconsumer_2015-02-05_20.11'
'/home/andersg/HLRS/results/producerconsumer_2015-02-05_21.03'
'/home/andersg/HLRS/results/producerconsumer_2015-02-05_21.56'
'/home/andersg/HLRS/results/producerconsumer_2015-02-05_22.48'
'/home/andersg/HLRS/results/producerconsumer_2015-02-05_23.40'
'/home/andersg/HLRS/results/producerconsumer_2015-02-06_01.05'
'/home/andersg/HLRS/results/producerconsumer_2015-02-06_01.58'
'/home/andersg/HLRS/results/producerconsumer_2015-02-06_02.50'
'/home/andersg/HLRS/results/producerconsumer_2015-02-06_03.42'
'/home/andersg/HLRS/results/producerconsumer_2015-02-06_04.34'
'/home/andersg/HLRS/results/producerconsumer_2015-02-06_05.27'
'/home/andersg/HLRS/results/producerconsumer_2015-02-06_06.19'
'/home/andersg/HLRS/results/producerconsumer_2015-02-06_07.11'
'/home/andersg/HLRS/results/producerconsumer_2015-02-06_08.04'
'/home/andersg/HLRS/results/producerconsumer_2015-02-06_08.56'
       ];

%% Algorithms: 0 2 3 6 10; Threads: 2 10 12 20; Pinning: 1
RUNs3 = [
'/home/andersg/HLRS/results/producerconsumer_2015-03-06_17.28'
'/home/andersg/HLRS/results/producerconsumer_2015-03-06_18.20'
'/home/andersg/HLRS/results/producerconsumer_2015-03-06_19.13'
'/home/andersg/HLRS/results/producerconsumer_2015-03-06_20.05'
'/home/andersg/HLRS/results/producerconsumer_2015-03-06_20.57'
'/home/andersg/HLRS/results/producerconsumer_2015-03-06_21.51'
'/home/andersg/HLRS/results/producerconsumer_2015-03-06_22.40'
'/home/andersg/HLRS/results/producerconsumer_2015-03-06_23.29'
'/home/andersg/HLRS/results/producerconsumer_2015-03-07_00.51'
'/home/andersg/HLRS/results/producerconsumer_2015-03-07_01.40'
'/home/andersg/HLRS/results/producerconsumer_2015-03-07_02.29'
'/home/andersg/HLRS/results/producerconsumer_2015-03-07_03.18'
'/home/andersg/HLRS/results/producerconsumer_2015-03-07_04.07'
'/home/andersg/HLRS/results/producerconsumer_2015-03-07_04.56'
'/home/andersg/HLRS/results/producerconsumer_2015-03-07_05.45'
'/home/andersg/HLRS/results/producerconsumer_2015-03-07_06.34'
'/home/andersg/HLRS/results/producerconsumer_2015-03-07_07.23'
'/home/andersg/HLRS/results/producerconsumer_2015-03-07_08.12'
'/home/andersg/HLRS/results/producerconsumer_2015-03-07_09.01'
'/home/andersg/HLRS/results/producerconsumer_2015-03-07_09.50'
       ];

%% Algorithms: 0 2 3 6 10; Threads: 2 10 12 20; Pinning: 2
RUNs4 = [
'/home/andersg/HLRS/results/producerconsumer_2015-03-10_16.09'
'/home/andersg/HLRS/results/producerconsumer_2015-03-10_17.02'
'/home/andersg/HLRS/results/producerconsumer_2015-03-10_17.54'
'/home/andersg/HLRS/results/producerconsumer_2015-03-10_18.47'
'/home/andersg/HLRS/results/producerconsumer_2015-03-10_19.39'
'/home/andersg/HLRS/results/producerconsumer_2015-03-10_20.31'
'/home/andersg/HLRS/results/producerconsumer_2015-03-10_21.24'
'/home/andersg/HLRS/results/producerconsumer_2015-03-10_22.16'
'/home/andersg/HLRS/results/producerconsumer_2015-03-10_23.08'
'/home/andersg/HLRS/results/producerconsumer_2015-03-11_00.01'
'/home/andersg/HLRS/results/producerconsumer_2015-03-11_01.26'
'/home/andersg/HLRS/results/producerconsumer_2015-03-11_02.18'
'/home/andersg/HLRS/results/producerconsumer_2015-03-11_03.11'
'/home/andersg/HLRS/results/producerconsumer_2015-03-11_04.03'
'/home/andersg/HLRS/results/producerconsumer_2015-03-11_04.56'
'/home/andersg/HLRS/results/producerconsumer_2015-03-11_05.48'
'/home/andersg/HLRS/results/producerconsumer_2015-03-11_06.40'
'/home/andersg/HLRS/results/producerconsumer_2015-03-11_07.33'
'/home/andersg/HLRS/results/producerconsumer_2015-03-11_08.25'
'/home/andersg/HLRS/results/producerconsumer_2015-03-11_09.17'
       ];

%% Algorithms: 0 1 2 3 4 5 6 7 8 11; Threads: 20; Pinning: 1; PPW=CPW=1
RUNs5 = [
'/home/andersg/HLRS/results/producerconsumer_2015-11-12_12.21'
         ];

%% Algorithms: 0 1 2 3 4 5 6 7 8; Threads: 2 4 6 8 10 12 14 16 18 20; Pinning: 1;
%% PPW=CPW: 1 2 3 4 5 7 10 20 50 150 500 1000
%% rdtscp instruction in the parallel work.
RUNs6 = [
% Alg 0
'/home/andersg/HLRS/results/producerconsumer_2015-11-12_16.55'
'/home/andersg/HLRS/results/producerconsumer_2015-11-12_17.44'
'/home/andersg/HLRS/results/producerconsumer_2015-11-12_18.34'
'/home/andersg/HLRS/results/producerconsumer_2015-11-12_19.22'
'/home/andersg/HLRS/results/producerconsumer_2015-11-12_20.11'
'/home/andersg/HLRS/results/producerconsumer_2015-11-12_21.00'
'/home/andersg/HLRS/results/producerconsumer_2015-11-12_21.49'
'/home/andersg/HLRS/results/producerconsumer_2015-11-12_22.38'
'/home/andersg/HLRS/results/producerconsumer_2015-11-12_23.27'
'/home/andersg/HLRS/results/producerconsumer_2015-11-13_00.15'
% Alg 1
'/home/andersg/HLRS/results/producerconsumer_2015-11-15_04.12'
'/home/andersg/HLRS/results/producerconsumer_2015-11-15_05.01'
'/home/andersg/HLRS/results/producerconsumer_2015-11-15_05.50'
'/home/andersg/HLRS/results/producerconsumer_2015-11-15_06.39'
'/home/andersg/HLRS/results/producerconsumer_2015-11-15_07.27'
'/home/andersg/HLRS/results/producerconsumer_2015-11-15_08.16'
'/home/andersg/HLRS/results/producerconsumer_2015-11-15_09.05'
'/home/andersg/HLRS/results/producerconsumer_2015-11-15_09.54'
'/home/andersg/HLRS/results/producerconsumer_2015-11-15_10.43'
'/home/andersg/HLRS/results/producerconsumer_2015-11-15_11.32'
% Alg 2
'/home/andersg/HLRS/results/producerconsumer_2015-11-15_12.20'
'/home/andersg/HLRS/results/producerconsumer_2015-11-15_13.09'
'/home/andersg/HLRS/results/producerconsumer_2015-11-15_13.58'
'/home/andersg/HLRS/results/producerconsumer_2015-11-15_14.47'
'/home/andersg/HLRS/results/producerconsumer_2015-11-15_15.36'
'/home/andersg/HLRS/results/producerconsumer_2015-11-15_16.25'
'/home/andersg/HLRS/results/producerconsumer_2015-11-15_17.13'
'/home/andersg/HLRS/results/producerconsumer_2015-11-15_18.02'
'/home/andersg/HLRS/results/producerconsumer_2015-11-15_18.51'
'/home/andersg/HLRS/results/producerconsumer_2015-11-15_19.40'
% Alg 3
'/home/andersg/HLRS/results/producerconsumer_2015-11-15_20.29'
'/home/andersg/HLRS/results/producerconsumer_2015-11-15_21.17'
'/home/andersg/HLRS/results/producerconsumer_2015-11-15_22.06'
'/home/andersg/HLRS/results/producerconsumer_2015-11-15_22.55'
'/home/andersg/HLRS/results/producerconsumer_2015-11-15_23.44'
'/home/andersg/HLRS/results/producerconsumer_2015-11-16_00.33'
'/home/andersg/HLRS/results/producerconsumer_2015-11-16_01.22'
'/home/andersg/HLRS/results/producerconsumer_2015-11-16_02.10'
'/home/andersg/HLRS/results/producerconsumer_2015-11-16_02.59'
'/home/andersg/HLRS/results/producerconsumer_2015-11-16_03.48'
% Alg 4
'/home/andersg/HLRS/results/producerconsumer_2015-11-16_04.37'
'/home/andersg/HLRS/results/producerconsumer_2015-11-16_05.26'
'/home/andersg/HLRS/results/producerconsumer_2015-11-16_06.15'
'/home/andersg/HLRS/results/producerconsumer_2015-11-16_07.03'
'/home/andersg/HLRS/results/producerconsumer_2015-11-16_07.52'
'/home/andersg/HLRS/results/producerconsumer_2015-11-16_08.41'
'/home/andersg/HLRS/results/producerconsumer_2015-11-16_09.30'
'/home/andersg/HLRS/results/producerconsumer_2015-11-16_10.19'
'/home/andersg/HLRS/results/producerconsumer_2015-11-16_11.08'
'/home/andersg/HLRS/results/producerconsumer_2015-11-16_11.56'
% Alg 5
'/home/andersg/HLRS/results/producerconsumer_2015-11-16_12.45'
'/home/andersg/HLRS/results/producerconsumer_2015-11-16_13.34'
'/home/andersg/HLRS/results/producerconsumer_2015-11-16_14.23'
'/home/andersg/HLRS/results/producerconsumer_2015-11-16_15.12'
'/home/andersg/HLRS/results/producerconsumer_2015-11-16_16.13'
'/home/andersg/HLRS/results/producerconsumer_2015-11-16_17.14'
'/home/andersg/HLRS/results/producerconsumer_2015-11-16_18.05'
'/home/andersg/HLRS/results/producerconsumer_2015-11-16_18.54'
'/home/andersg/HLRS/results/producerconsumer_2015-11-16_19.43'
'/home/andersg/HLRS/results/producerconsumer_2015-11-16_20.32'
% Alg 6
'/home/andersg/HLRS/results/producerconsumer_2015-11-16_21.21'
'/home/andersg/HLRS/results/producerconsumer_2015-11-16_22.09'
'/home/andersg/HLRS/results/producerconsumer_2015-11-16_23.10'
'/home/andersg/HLRS/results/producerconsumer_2015-11-17_00.13'
'/home/andersg/HLRS/results/producerconsumer_2015-11-17_01.14'
'/home/andersg/HLRS/results/producerconsumer_2015-11-17_02.16'
'/home/andersg/HLRS/results/producerconsumer_2015-11-17_03.17'
'/home/andersg/HLRS/results/producerconsumer_2015-11-17_04.18'
'/home/andersg/HLRS/results/producerconsumer_2015-11-17_05.20'
'/home/andersg/HLRS/results/producerconsumer_2015-11-17_06.22'
% Alg 7
'/home/andersg/HLRS/results/producerconsumer_2015-11-17_07.25'
'/home/andersg/HLRS/results/producerconsumer_2015-11-17_08.28'
'/home/andersg/HLRS/results/producerconsumer_2015-11-17_09.29'
'/home/andersg/HLRS/results/producerconsumer_2015-11-17_10.18'
'/home/andersg/HLRS/results/producerconsumer_2015-11-13_12.20'
'/home/andersg/HLRS/results/producerconsumer_2015-11-17_11.16'
'/home/andersg/HLRS/results/producerconsumer_2015-11-17_12.17'
'/home/andersg/HLRS/results/producerconsumer_2015-11-17_13.07'
'/home/andersg/HLRS/results/producerconsumer_2015-11-17_13.56'
'/home/andersg/HLRS/results/producerconsumer_2015-11-17_14.45'
% Alg 8
'/home/andersg/HLRS/results/producerconsumer_2015-11-17_15.36'
'/home/andersg/HLRS/results/producerconsumer_2015-11-17_16.25'
'/home/andersg/HLRS/results/producerconsumer_2015-11-17_17.18'
'/home/andersg/HLRS/results/producerconsumer_2015-11-17_18.06'
'/home/andersg/HLRS/results/producerconsumer_2015-11-17_18.55'
'/home/andersg/HLRS/results/producerconsumer_2015-11-17_19.44'
'/home/andersg/HLRS/results/producerconsumer_2015-11-17_20.33'
'/home/andersg/HLRS/results/producerconsumer_2015-11-17_21.22'
'/home/andersg/HLRS/results/producerconsumer_2015-11-17_22.11'
'/home/andersg/HLRS/results/producerconsumer_2015-11-17_23.00'
         ];

%% Algorithms: 0 1 2 3 4 5 6 7 8; Threads: 2 4 6 8 10 12 14 16 18 20; Pinning: 1; 
%% PPW=CPW: 1 2 3 4 5 7 10 20 50 150 500 1000
%% Half as many rdtscp instructions in the parallel work.
RUNs7 = [
% Alg 0
'/home/andersg/HLRS/results/producerconsumer_2015-11-21_20.06'
'/home/andersg/HLRS/results/producerconsumer_2015-11-21_20.55'
'/home/andersg/HLRS/results/producerconsumer_2015-11-21_21.43'
'/home/andersg/HLRS/results/producerconsumer_2015-11-21_22.32'
'/home/andersg/HLRS/results/producerconsumer_2015-11-21_23.21'
'/home/andersg/HLRS/results/producerconsumer_2015-11-22_00.10'
'/home/andersg/HLRS/results/producerconsumer_2015-11-22_00.59'
'/home/andersg/HLRS/results/producerconsumer_2015-11-22_01.48'
'/home/andersg/HLRS/results/producerconsumer_2015-11-22_02.37'
'/home/andersg/HLRS/results/producerconsumer_2015-11-22_03.26'
% Alg 1
'/home/andersg/HLRS/results/producerconsumer_2015-11-22_04.14'
'/home/andersg/HLRS/results/producerconsumer_2015-11-22_05.03'
'/home/andersg/HLRS/results/producerconsumer_2015-11-22_05.52'
'/home/andersg/HLRS/results/producerconsumer_2015-11-22_06.41'
'/home/andersg/HLRS/results/producerconsumer_2015-11-22_07.30'
'/home/andersg/HLRS/results/producerconsumer_2015-11-22_08.19'
'/home/andersg/HLRS/results/producerconsumer_2015-11-22_09.07'
'/home/andersg/HLRS/results/producerconsumer_2015-11-22_09.56'
'/home/andersg/HLRS/results/producerconsumer_2015-11-22_10.45'
'/home/andersg/HLRS/results/producerconsumer_2015-11-22_11.34'
% Alg 2
'/home/andersg/HLRS/results/producerconsumer_2015-11-22_12.23'
'/home/andersg/HLRS/results/producerconsumer_2015-11-22_13.12'
'/home/andersg/HLRS/results/producerconsumer_2015-11-22_14.01'
'/home/andersg/HLRS/results/producerconsumer_2015-11-22_14.50'
'/home/andersg/HLRS/results/producerconsumer_2015-11-22_15.39'
'/home/andersg/HLRS/results/producerconsumer_2015-11-22_16.27'
'/home/andersg/HLRS/results/producerconsumer_2015-11-22_17.16'
'/home/andersg/HLRS/results/producerconsumer_2015-11-22_18.05'
'/home/andersg/HLRS/results/producerconsumer_2015-11-22_18.54'
'/home/andersg/HLRS/results/producerconsumer_2015-11-22_19.43'
% Alg 3
'/home/andersg/HLRS/results/producerconsumer_2015-11-22_20.31'
'/home/andersg/HLRS/results/producerconsumer_2015-11-22_21.20'
'/home/andersg/HLRS/results/producerconsumer_2015-11-22_22.09'
'/home/andersg/HLRS/results/producerconsumer_2015-11-22_22.58'
'/home/andersg/HLRS/results/producerconsumer_2015-11-22_23.47'
'/home/andersg/HLRS/results/producerconsumer_2015-11-23_00.36'
'/home/andersg/HLRS/results/producerconsumer_2015-11-23_01.24'
'/home/andersg/HLRS/results/producerconsumer_2015-11-23_02.13'
'/home/andersg/HLRS/results/producerconsumer_2015-11-23_03.02'
'/home/andersg/HLRS/results/producerconsumer_2015-11-23_03.51'
% Alg 4
'/home/andersg/HLRS/results/producerconsumer_2015-11-23_04.40'
'/home/andersg/HLRS/results/producerconsumer_2015-11-23_05.29'
'/home/andersg/HLRS/results/producerconsumer_2015-11-23_06.17'
'/home/andersg/HLRS/results/producerconsumer_2015-11-23_07.06'
'/home/andersg/HLRS/results/producerconsumer_2015-11-23_07.55'
'/home/andersg/HLRS/results/producerconsumer_2015-11-23_08.44'
'/home/andersg/HLRS/results/producerconsumer_2015-11-23_09.33'
'/home/andersg/HLRS/results/producerconsumer_2015-11-23_10.22'
'/home/andersg/HLRS/results/producerconsumer_2015-11-23_11.17'
'/home/andersg/HLRS/results/producerconsumer_2015-11-23_12.06'
% Alg 5
'/home/andersg/HLRS/results/producerconsumer_2015-11-23_12.55'
'/home/andersg/HLRS/results/producerconsumer_2015-11-23_13.44'
'/home/andersg/HLRS/results/producerconsumer_2015-11-23_14.33'
'/home/andersg/HLRS/results/producerconsumer_2015-11-23_15.22'
'/home/andersg/HLRS/results/producerconsumer_2015-11-23_16.12'
'/home/andersg/HLRS/results/producerconsumer_2015-11-23_17.01'
'/home/andersg/HLRS/results/producerconsumer_2015-11-23_18.01'
'/home/andersg/HLRS/results/producerconsumer_2015-11-23_18.50'
'/home/andersg/HLRS/results/producerconsumer_2015-11-23_19.39'
'/home/andersg/HLRS/results/producerconsumer_2015-11-23_20.28'
% Alg 6
'/home/andersg/HLRS/results/producerconsumer_2015-11-23_21.28'
'/home/andersg/HLRS/results/producerconsumer_2015-11-23_22.17'
'/home/andersg/HLRS/results/producerconsumer_2015-11-23_23.06'
'/home/andersg/HLRS/results/producerconsumer_2015-11-23_23.55'
'/home/andersg/HLRS/results/producerconsumer_2015-11-24_00.44'
'/home/andersg/HLRS/results/producerconsumer_2015-11-24_01.33'
'/home/andersg/HLRS/results/producerconsumer_2015-11-24_02.22'
'/home/andersg/HLRS/results/producerconsumer_2015-11-24_03.11'
'/home/andersg/HLRS/results/producerconsumer_2015-11-24_03.59'
'/home/andersg/HLRS/results/producerconsumer_2015-11-24_04.48'
% Alg 7
'/home/andersg/HLRS/results/producerconsumer_2015-11-24_05.37'
'/home/andersg/HLRS/results/producerconsumer_2015-11-24_06.26'
'/home/andersg/HLRS/results/producerconsumer_2015-11-24_07.15'
'/home/andersg/HLRS/results/producerconsumer_2015-11-24_08.04'
'/home/andersg/HLRS/results/producerconsumer_2015-11-24_08.52'
'/home/andersg/HLRS/results/producerconsumer_2015-11-24_09.41'
'/home/andersg/HLRS/results/producerconsumer_2015-11-24_10.40'
'/home/andersg/HLRS/results/producerconsumer_2015-11-24_14.00'
'/home/andersg/HLRS/results/producerconsumer_2015-11-24_15.35'
'/home/andersg/HLRS/results/producerconsumer_2015-11-24_16.24'
% Alg 8
'/home/andersg/HLRS/results/producerconsumer_2015-11-24_17.34'
'/home/andersg/HLRS/results/producerconsumer_2015-11-24_18.23'
'/home/andersg/HLRS/results/producerconsumer_2015-11-24_19.11'
'/home/andersg/HLRS/results/producerconsumer_2015-11-24_20.00'
'/home/andersg/HLRS/results/producerconsumer_2015-11-24_20.49'
'/home/andersg/HLRS/results/producerconsumer_2015-11-24_21.38'
'/home/andersg/HLRS/results/producerconsumer_2015-11-24_22.27'
'/home/andersg/HLRS/results/producerconsumer_2015-11-24_23.16'
'/home/andersg/HLRS/results/producerconsumer_2015-11-25_00.05'
'/home/andersg/HLRS/results/producerconsumer_2015-11-25_00.54'
         ];

%% Algorithms: 0; Threads: 20; Pinning: 1; Pattern: 2;
%% PPW=CPW: 1 2 3 4 5 7 10 20 50 150 500 1000
%% Half as many rdtscp instructions in the parallel work.
RUNs8 = [
% Alg 0
'/home/andersg/HLRS/results/producerconsumer_2015-11-27_16.16'
         ];

%% Algorithms: 0; Threads: 20; Pinning: 1; Pattern: 3;
%% PPW=CPW: 1 2 3 4 5 7 10 20 50 150 500 1000
%% Half as many rdtscp instructions in the parallel work.
RUNs9 = [
% Alg 0
'/home/andersg/HLRS/results/producerconsumer_2015-11-27_17.05'
         ];

%% Algorithms: 0; Threads: 20; Pinning: 1; Pattern: 3 4 5 6 7 (do_enq=tiD%pattern);
%% PPW=CPW: 1 2 3 4 5 7 10 20 50 150 500 1000
%% Half as many rdtscp instructions in the parallel work.
RUNs10 = [
'/home/andersg/HLRS/results/producerconsumer_2015-12-14_22.20'
'/home/andersg/HLRS/results/producerconsumer_2015-12-14_23.09'
'/home/andersg/HLRS/results/producerconsumer_2015-12-14_23.57'
'/home/andersg/HLRS/results/producerconsumer_2015-12-15_00.46'
'/home/andersg/HLRS/results/producerconsumer_2015-12-15_01.35'
         ];

%% Algorithms: 0; Threads: 20; Pinning: 1; Pattern: 2 3 4 5 6 7 (do_enq=tiD%pattern);
%% PPW=CPW: 1 2 3 4 5 7 10 20 50 150 500 1000
%% No rdtscp instructions in the parallel work.
RUNs11 = [
'/home/andersg/HLRS/results/producerconsumer_2015-12-18_17.50'
'/home/andersg/HLRS/results/producerconsumer_2015-12-18_18.39'
'/home/andersg/HLRS/results/producerconsumer_2015-12-18_14.35'
'/home/andersg/HLRS/results/producerconsumer_2015-12-18_15.24'
'/home/andersg/HLRS/results/producerconsumer_2015-12-18_16.12'
'/home/andersg/HLRS/results/producerconsumer_2015-12-18_17.01'
         ];

%% Algorithms: 0 1 2 3 4 5 6 7 8; Threads: 2 4 6 8 10 12 14 16 18 20; Pinning: 1; Pattern: 1 (#producers = #consumers)
%% PPW=CPW: 1 2 3 4 5 7 10 20 50 150 500 1000
%% No rdtscp instructions in the parallel work.
RUNs12 = [
'/home/andersg/HLRS/results/producerconsumer_2016-01-05_09.51'
'/home/andersg/HLRS/results/producerconsumer_2016-01-05_10.40'
'/home/andersg/HLRS/results/producerconsumer_2016-01-05_11.28'
'/home/andersg/HLRS/results/producerconsumer_2016-01-05_12.17'
'/home/andersg/HLRS/results/producerconsumer_2016-01-05_13.06'
'/home/andersg/HLRS/results/producerconsumer_2016-01-05_13.55'
'/home/andersg/HLRS/results/producerconsumer_2016-01-05_14.43'
'/home/andersg/HLRS/results/producerconsumer_2016-01-05_15.32'
'/home/andersg/HLRS/results/producerconsumer_2016-01-05_16.21'
'/home/andersg/HLRS/results/producerconsumer_2016-01-05_17.10'
'/home/andersg/HLRS/results/producerconsumer_2016-01-05_17.58'
'/home/andersg/HLRS/results/producerconsumer_2016-01-05_18.47'
'/home/andersg/HLRS/results/producerconsumer_2016-01-05_19.36'
'/home/andersg/HLRS/results/producerconsumer_2016-01-05_20.25'
'/home/andersg/HLRS/results/producerconsumer_2016-01-05_21.13'
'/home/andersg/HLRS/results/producerconsumer_2016-01-05_22.02'
'/home/andersg/HLRS/results/producerconsumer_2016-01-05_22.51'
'/home/andersg/HLRS/results/producerconsumer_2016-01-05_23.40'
'/home/andersg/HLRS/results/producerconsumer_2016-01-06_00.28'
'/home/andersg/HLRS/results/producerconsumer_2016-01-06_01.17'
'/home/andersg/HLRS/results/producerconsumer_2016-01-06_02.06'
'/home/andersg/HLRS/results/producerconsumer_2016-01-06_02.55'
'/home/andersg/HLRS/results/producerconsumer_2016-01-06_03.43'
'/home/andersg/HLRS/results/producerconsumer_2016-01-06_04.32'
'/home/andersg/HLRS/results/producerconsumer_2016-01-06_05.21'
'/home/andersg/HLRS/results/producerconsumer_2016-01-06_06.10'
'/home/andersg/HLRS/results/producerconsumer_2016-01-06_06.58'
'/home/andersg/HLRS/results/producerconsumer_2016-01-06_07.47'
'/home/andersg/HLRS/results/producerconsumer_2016-01-06_08.36'
'/home/andersg/HLRS/results/producerconsumer_2016-01-06_09.25'
'/home/andersg/HLRS/results/producerconsumer_2016-01-06_10.13'
'/home/andersg/HLRS/results/producerconsumer_2016-01-06_11.02'
'/home/andersg/HLRS/results/producerconsumer_2016-01-06_11.51'
'/home/andersg/HLRS/results/producerconsumer_2016-01-06_12.40'
'/home/andersg/HLRS/results/producerconsumer_2016-01-06_13.28'
'/home/andersg/HLRS/results/producerconsumer_2016-01-06_14.17'
'/home/andersg/HLRS/results/producerconsumer_2016-01-06_15.06'
'/home/andersg/HLRS/results/producerconsumer_2016-01-06_15.55'
'/home/andersg/HLRS/results/producerconsumer_2016-01-06_16.43'
'/home/andersg/HLRS/results/producerconsumer_2016-01-06_17.32'
'/home/andersg/HLRS/results/producerconsumer_2016-01-06_18.21'
'/home/andersg/HLRS/results/producerconsumer_2016-01-06_19.10'
'/home/andersg/HLRS/results/producerconsumer_2016-01-06_19.58'
'/home/andersg/HLRS/results/producerconsumer_2016-01-06_20.47'
'/home/andersg/HLRS/results/producerconsumer_2016-01-06_21.36'
'/home/andersg/HLRS/results/producerconsumer_2016-01-06_22.25'
'/home/andersg/HLRS/results/producerconsumer_2016-01-06_23.14'
'/home/andersg/HLRS/results/producerconsumer_2016-01-07_00.02'
'/home/andersg/HLRS/results/producerconsumer_2016-01-07_00.51'
'/home/andersg/HLRS/results/producerconsumer_2016-01-07_01.40'
'/home/andersg/HLRS/results/producerconsumer_2016-01-07_02.29'
'/home/andersg/HLRS/results/producerconsumer_2016-01-07_03.17'
'/home/andersg/HLRS/results/producerconsumer_2016-01-07_04.06'
'/home/andersg/HLRS/results/producerconsumer_2016-01-07_04.55'
'/home/andersg/HLRS/results/producerconsumer_2016-01-07_05.44'
'/home/andersg/HLRS/results/producerconsumer_2016-01-07_06.32'
'/home/andersg/HLRS/results/producerconsumer_2016-01-07_07.21'
'/home/andersg/HLRS/results/producerconsumer_2016-01-07_08.10'
'/home/andersg/HLRS/results/producerconsumer_2016-01-07_08.59'
'/home/andersg/HLRS/results/producerconsumer_2016-01-07_09.47'
'/home/andersg/HLRS/results/producerconsumer_2016-01-07_10.36'
'/home/andersg/HLRS/results/producerconsumer_2016-01-07_11.25'
'/home/andersg/HLRS/results/producerconsumer_2016-01-07_12.14'
'/home/andersg/HLRS/results/producerconsumer_2016-01-07_13.02'
'/home/andersg/HLRS/results/producerconsumer_2016-01-07_13.51'
'/home/andersg/HLRS/results/producerconsumer_2016-01-07_14.40'
'/home/andersg/HLRS/results/producerconsumer_2016-01-07_15.29'
'/home/andersg/HLRS/results/producerconsumer_2016-01-07_16.17'
'/home/andersg/HLRS/results/producerconsumer_2016-01-07_17.06'
'/home/andersg/HLRS/results/producerconsumer_2016-01-07_17.55'
'/home/andersg/HLRS/results/producerconsumer_2016-01-07_18.44'
'/home/andersg/HLRS/results/producerconsumer_2016-01-07_19.32'
'/home/andersg/HLRS/results/producerconsumer_2016-01-07_20.21'
'/home/andersg/HLRS/results/producerconsumer_2016-01-07_21.10'
'/home/andersg/HLRS/results/producerconsumer_2016-01-07_21.59'
'/home/andersg/HLRS/results/producerconsumer_2016-01-07_22.47'
'/home/andersg/HLRS/results/producerconsumer_2016-01-07_23.36'
'/home/andersg/HLRS/results/producerconsumer_2016-01-08_00.25'
'/home/andersg/HLRS/results/producerconsumer_2016-01-08_01.14'
'/home/andersg/HLRS/results/producerconsumer_2016-01-08_02.02'
'/home/andersg/HLRS/results/producerconsumer_2016-01-08_02.51'
'/home/andersg/HLRS/results/producerconsumer_2016-01-08_03.40'
'/home/andersg/HLRS/results/producerconsumer_2016-01-08_04.29'
'/home/andersg/HLRS/results/producerconsumer_2016-01-08_05.17'
'/home/andersg/HLRS/results/producerconsumer_2016-01-08_06.06'
'/home/andersg/HLRS/results/producerconsumer_2016-01-08_06.55'
'/home/andersg/HLRS/results/producerconsumer_2016-01-08_07.44'
'/home/andersg/HLRS/results/producerconsumer_2016-01-08_08.32'
'/home/andersg/HLRS/results/producerconsumer_2016-01-08_09.21'
'/home/andersg/HLRS/results/producerconsumer_2016-01-08_10.10'
          ];

%% Algorithms: 0 1 2 3 4 5 6 7 8 9 10 11 12; Threads: 20; Pinning: 1; Pattern: 2 3 4 5 7 8 9 26 27 28 ("1:N-1" "N-1:1" "0:N" "N:0" "do_enq = myId % {3,4,5}" "do_enq = !(myId % {3,4,5})")
%% PPW=CPW: 1 2 3 4 5 7 10 20 50 150 500 1000
%% No rdtscp instructions in the parallel work.
%% Total #cases: 18720
RUNs13 = [
'/home/andersg/HLRS/results/producerconsumer_2016-02-18_17.08'
'/home/andersg/HLRS/results/producerconsumer_2016-02-18_17.57'
'/home/andersg/HLRS/results/producerconsumer_2016-02-18_18.45'
'/home/andersg/HLRS/results/producerconsumer_2016-02-18_19.34'
'/home/andersg/HLRS/results/producerconsumer_2016-02-18_20.23'
'/home/andersg/HLRS/results/producerconsumer_2016-02-18_21.12'
'/home/andersg/HLRS/results/producerconsumer_2016-02-18_22.00'
'/home/andersg/HLRS/results/producerconsumer_2016-02-18_22.49'
'/home/andersg/HLRS/results/producerconsumer_2016-02-18_23.38'
'/home/andersg/HLRS/results/producerconsumer_2016-02-19_00.27'
'/home/andersg/HLRS/results/producerconsumer_2016-02-19_01.15'
'/home/andersg/HLRS/results/producerconsumer_2016-02-19_02.04'
'/home/andersg/HLRS/results/producerconsumer_2016-02-19_02.53'
'/home/andersg/HLRS/results/producerconsumer_2016-02-19_03.42'
'/home/andersg/HLRS/results/producerconsumer_2016-02-19_04.30'
'/home/andersg/HLRS/results/producerconsumer_2016-02-19_05.19'
'/home/andersg/HLRS/results/producerconsumer_2016-02-19_06.08'
'/home/andersg/HLRS/results/producerconsumer_2016-02-19_06.57'
'/home/andersg/HLRS/results/producerconsumer_2016-02-19_07.46'
'/home/andersg/HLRS/results/producerconsumer_2016-02-19_08.34'
'/home/andersg/HLRS/results/producerconsumer_2016-02-19_09.23'
'/home/andersg/HLRS/results/producerconsumer_2016-02-19_10.12'
'/home/andersg/HLRS/results/producerconsumer_2016-02-19_11.01'
'/home/andersg/HLRS/results/producerconsumer_2016-02-19_11.49'
'/home/andersg/HLRS/results/producerconsumer_2016-02-19_12.38'
'/home/andersg/HLRS/results/producerconsumer_2016-02-19_13.27'
'/home/andersg/HLRS/results/producerconsumer_2016-02-19_14.16'
'/home/andersg/HLRS/results/producerconsumer_2016-02-19_15.04'
'/home/andersg/HLRS/results/producerconsumer_2016-02-19_15.54'
'/home/andersg/HLRS/results/producerconsumer_2016-02-19_16.43'
'/home/andersg/HLRS/results/producerconsumer_2016-02-19_17.31'
'/home/andersg/HLRS/results/producerconsumer_2016-02-19_18.20'
'/home/andersg/HLRS/results/producerconsumer_2016-02-19_19.09'
'/home/andersg/HLRS/results/producerconsumer_2016-02-19_19.58'
'/home/andersg/HLRS/results/producerconsumer_2016-02-19_20.46'
'/home/andersg/HLRS/results/producerconsumer_2016-02-19_21.35'
'/home/andersg/HLRS/results/producerconsumer_2016-02-19_22.24'
'/home/andersg/HLRS/results/producerconsumer_2016-02-19_23.13'
'/home/andersg/HLRS/results/producerconsumer_2016-02-20_00.01'
'/home/andersg/HLRS/results/producerconsumer_2016-02-20_00.50'
'/home/andersg/HLRS/results/producerconsumer_2016-02-20_01.39'
'/home/andersg/HLRS/results/producerconsumer_2016-02-20_02.28'
'/home/andersg/HLRS/results/producerconsumer_2016-02-20_03.16'
'/home/andersg/HLRS/results/producerconsumer_2016-02-20_04.05'
'/home/andersg/HLRS/results/producerconsumer_2016-02-20_04.54'
'/home/andersg/HLRS/results/producerconsumer_2016-02-20_05.43'
'/home/andersg/HLRS/results/producerconsumer_2016-02-20_06.31'
'/home/andersg/HLRS/results/producerconsumer_2016-02-20_07.20'
'/home/andersg/HLRS/results/producerconsumer_2016-02-20_08.09'
'/home/andersg/HLRS/results/producerconsumer_2016-02-20_08.58'
'/home/andersg/HLRS/results/producerconsumer_2016-02-20_09.46'
'/home/andersg/HLRS/results/producerconsumer_2016-02-20_10.35'
'/home/andersg/HLRS/results/producerconsumer_2016-02-20_11.24'
'/home/andersg/HLRS/results/producerconsumer_2016-02-20_12.13'
'/home/andersg/HLRS/results/producerconsumer_2016-02-20_13.01'
'/home/andersg/HLRS/results/producerconsumer_2016-02-20_13.50'
'/home/andersg/HLRS/results/producerconsumer_2016-02-20_14.39'
'/home/andersg/HLRS/results/producerconsumer_2016-02-20_15.28'
'/home/andersg/HLRS/results/producerconsumer_2016-02-20_16.17'
'/home/andersg/HLRS/results/producerconsumer_2016-02-20_17.05'
'/home/andersg/HLRS/results/producerconsumer_2016-02-20_17.54'
'/home/andersg/HLRS/results/producerconsumer_2016-02-20_18.43'
'/home/andersg/HLRS/results/producerconsumer_2016-02-20_19.32'
'/home/andersg/HLRS/results/producerconsumer_2016-02-20_20.20'
'/home/andersg/HLRS/results/producerconsumer_2016-02-20_21.09'
'/home/andersg/HLRS/results/producerconsumer_2016-02-20_21.58'
'/home/andersg/HLRS/results/producerconsumer_2016-02-20_22.47'
'/home/andersg/HLRS/results/producerconsumer_2016-02-20_23.35'
'/home/andersg/HLRS/results/producerconsumer_2016-02-21_00.24'
'/home/andersg/HLRS/results/producerconsumer_2016-02-21_01.13'
'/home/andersg/HLRS/results/producerconsumer_2016-02-21_02.02'
'/home/andersg/HLRS/results/producerconsumer_2016-02-21_02.50'
'/home/andersg/HLRS/results/producerconsumer_2016-02-21_03.39'
'/home/andersg/HLRS/results/producerconsumer_2016-02-21_04.28'
'/home/andersg/HLRS/results/producerconsumer_2016-02-21_05.17'
'/home/andersg/HLRS/results/producerconsumer_2016-02-21_06.05'
'/home/andersg/HLRS/results/producerconsumer_2016-02-21_06.54'
'/home/andersg/HLRS/results/producerconsumer_2016-02-21_07.43'
'/home/andersg/HLRS/results/producerconsumer_2016-02-21_08.32'
'/home/andersg/HLRS/results/producerconsumer_2016-02-21_09.20'
'/home/andersg/HLRS/results/producerconsumer_2016-02-21_10.09'
'/home/andersg/HLRS/results/producerconsumer_2016-02-21_10.58'
'/home/andersg/HLRS/results/producerconsumer_2016-02-21_11.47'
'/home/andersg/HLRS/results/producerconsumer_2016-02-21_12.35'
'/home/andersg/HLRS/results/producerconsumer_2016-02-21_13.24'
'/home/andersg/HLRS/results/producerconsumer_2016-02-21_14.13'
'/home/andersg/HLRS/results/producerconsumer_2016-02-21_15.02'
'/home/andersg/HLRS/results/producerconsumer_2016-02-21_15.50'
'/home/andersg/HLRS/results/producerconsumer_2016-02-21_16.39'
'/home/andersg/HLRS/results/producerconsumer_2016-02-21_17.28'
'/home/andersg/HLRS/results/producerconsumer_2016-02-21_18.17'
'/home/andersg/HLRS/results/producerconsumer_2016-02-21_19.05'
'/home/andersg/HLRS/results/producerconsumer_2016-02-21_19.54'
'/home/andersg/HLRS/results/producerconsumer_2016-02-21_20.43'
'/home/andersg/HLRS/results/producerconsumer_2016-02-21_21.32'
'/home/andersg/HLRS/results/producerconsumer_2016-02-21_22.20'
'/home/andersg/HLRS/results/producerconsumer_2016-02-21_23.09'
'/home/andersg/HLRS/results/producerconsumer_2016-02-21_23.58'
'/home/andersg/HLRS/results/producerconsumer_2016-02-22_00.47'
'/home/andersg/HLRS/results/producerconsumer_2016-02-22_01.35'
'/home/andersg/HLRS/results/producerconsumer_2016-02-22_02.24'
'/home/andersg/HLRS/results/producerconsumer_2016-02-22_03.13'
'/home/andersg/HLRS/results/producerconsumer_2016-02-22_04.02'
'/home/andersg/HLRS/results/producerconsumer_2016-02-22_04.50'
'/home/andersg/HLRS/results/producerconsumer_2016-02-22_05.39'
'/home/andersg/HLRS/results/producerconsumer_2016-02-22_06.28'
'/home/andersg/HLRS/results/producerconsumer_2016-02-22_07.17'
'/home/andersg/HLRS/results/producerconsumer_2016-02-22_08.06'
'/home/andersg/HLRS/results/producerconsumer_2016-02-22_08.54'
'/home/andersg/HLRS/results/producerconsumer_2016-02-22_09.43'
'/home/andersg/HLRS/results/producerconsumer_2016-02-22_10.32'
'/home/andersg/HLRS/results/producerconsumer_2016-02-22_11.21'
'/home/andersg/HLRS/results/producerconsumer_2016-02-22_12.09'
'/home/andersg/HLRS/results/producerconsumer_2016-02-22_12.58'
'/home/andersg/HLRS/results/producerconsumer_2016-02-22_13.47'
'/home/andersg/HLRS/results/producerconsumer_2016-02-22_14.36'
'/home/andersg/HLRS/results/producerconsumer_2016-02-22_15.24'
'/home/andersg/HLRS/results/producerconsumer_2016-02-22_16.13'
'/home/andersg/HLRS/results/producerconsumer_2016-02-22_17.02'
'/home/andersg/HLRS/results/producerconsumer_2016-02-22_17.51'
'/home/andersg/HLRS/results/producerconsumer_2016-02-22_18.39'
'/home/andersg/HLRS/results/producerconsumer_2016-02-22_19.28'
'/home/andersg/HLRS/results/producerconsumer_2016-02-22_20.17'
'/home/andersg/HLRS/results/producerconsumer_2016-02-22_21.06'
'/home/andersg/HLRS/results/producerconsumer_2016-02-22_21.54'
'/home/andersg/HLRS/results/producerconsumer_2016-02-22_22.43'
'/home/andersg/HLRS/results/producerconsumer_2016-02-22_23.32'
'/home/andersg/HLRS/results/producerconsumer_2016-02-23_00.21'
'/home/andersg/HLRS/results/producerconsumer_2016-02-23_01.09'
'/home/andersg/HLRS/results/producerconsumer_2016-02-23_01.58'
          ];

# Standard #producers=#consumers
PATTERNs=[1];
# Various distributions of #producers/#consumers
PATTERNs=[2 3 4 5 7 8 9 26 27 28];

RUNs = RUNs13;

i = 1;
res = [];
for d = 1:size(RUNs)(1)
  base = RUNs(d,:);
  basename = [base '/producerconsumer_result_' base(length(base) - 15 : length(base)) '-'];

    for f = FREQs
      for a = ALGs
        for pattern = PATTERNs
          algname = sprintf("a%d", a);

          for ppw = PPWs
            for cpw = CPWs
              for t = THREADs
                casename = sprintf("p%d-ppw%d-cpw%d-f%d-t%d", pattern, ppw, cpw, f, t);

                try
                  resfile = sprintf("%sOUT-%s-%s.txt", basename, algname, casename);
                  %printf("Trying '%s' ... ", outfile);

                  [info, err] = stat(resfile);
                  if (err == 0)
                    [alg threads pinning pattern pcpw throughputs RAPL_powers RAPL_powers_biased_coef_of_var] = summarize_producerconsumer_case(basename, algname, casename, PLOT_POWER);
                    %printf("succeeded\n");

                    res(i,:) = [f alg threads pinning pattern pcpw throughputs RAPL_powers RAPL_powers_biased_coef_of_var];
                    i = i+1;
                  else
                    %printf("failed\n");
                  endif
                catch
                  printf("exception in summarize_producerconsumer case '%s'\n", resfile);
                end_try_catch
              endfor
            endfor
          endfor
        endfor
      endfor
    endfor
endfor

%% Layout of result file:
%%   1-7:  freq alg threads pinning pattern ppw cpw
%%   8-10: tp_insert tp_try_remove_ok tp_try_remove_empty
%%  11-14: P_PKG_S1 P_PKG_S2 P_CPU_S1 P_CPU_S2
%%  15-18: P_Uncore_S1 P_Uncore_S2 P_Mem_S1 P_Mem_S2
%%  19-22: BCoV_PKG_S1 BCoV_PKG_S2 BCoV_CPU_S1 BCoV_CPU_S2
%%  23-26: BCoV_Uncore_S1 BCoV_Uncore_S2 BCoV_Mem_S1 BCoV_Mem_S2
csvwrite("result.res", res);
