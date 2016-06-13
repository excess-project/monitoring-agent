%% Comparison of MF and other data on the EXCESS cluster.

%% Experiment on node01.
%% ~/HLRS/results/producerconsumer_2015-06-02_16.58/au20x_cyochqsjvljjqn.json

basename = '../../../../HLRS/results/producerconsumer_2015-06-02_16.58/producerconsumer_result_2015-06-02_16.58-';

%% MF RAPL via likwid
[t_rapl1 PKG_power1 CPU_power1 UNCORE_power1 DRAM_power1] = load_MF_RAPL_power(basename);

%% MF external DAQ.
[t_daq PKG_power2 ATX12V_power GPU0_power] = load_MF_external_power(basename);

%% state-record-tool RAPL via likwid. Many cases to aggregate.
pinning = 1;
FREQs = [3];
ALGs = [10];
PPWs = [1 4 10 1000];
CPWs = [1 4 10 1000];
THREADs = [20];

t_rapl3 = [];
PKG_power3 = [];
for f = FREQs
  for a = ALGs
    algname = sprintf("a%d", a);

    for ppw = PPWs
      for cpw = CPWs
        for t = THREADs
          casename = sprintf("p1-ppw%d-cpw%d-f%d-t%d", ppw, cpw, f, t);

          try
            [t_rapl4 PKG_power4 CPU_power4 UNCORE_power4 DRAM_power4] = load_RAPL_power(basename, algname, casename);

            %% Join the per-case measurements
            t_rapl3 = [t_rapl3; t_rapl4;];
            PKG_power3 = [PKG_power3; PKG_power4;];
          catch
            printf("exception in MFverification case '%s'\n", casename);
          end_try_catch
        endfor
      endfor
    endfor
  endfor
endfor

%% Plot the PKG power data.
plot(t_rapl1, PKG_power1(:,1), "b+-;PKG S1 MF RAPL;",
     t_rapl1, PKG_power1(:,2), "g+-;PKG S2 MF RAPL;",
     t_daq,   PKG_power2(:,1), "rx-;PKG S1 External DAQ;",
     t_daq,   PKG_power2(:,2), "yx-;PKG S2 External DAQ;",
     t_rapl3, PKG_power3(:,1), "bo-;PKG S1 state-record-tool RAPL;",
     t_rapl3, PKG_power3(:,2), "go-;PKG S2 state-record-tool RAPL;");

title('CPU PKG/Socket power consumption - comparison of data sources.');
xlabel("Time instant (sec since the epoch)");
ylim([0, 70]);
ylabel("Power (W)");

print([basename algname '-' sprintf("pin%d",pinning) '-power.eps'], "-deps","-color");
