%% Summarize the result of one SpGEMM testbench case.
%% Anders Gidenstam  2016

function [alg threads pinning matrix mmalg wus durations operations RAPL_powers RAPL_powers_biased_coef_of_vars] = summarize_SpGEMM_case(basename, algname, casename, plot_power)

  res = load_case_result(basename, algname, casename);
  %% File format:
  %%  1-4:  <implementation#>  <#threads>  <pinning#>  <duration sec>
  %%  5-6:  <initialization delay sec>  <start time instant in seconds>
  %%  7-8:  <#Insert operations>  <#Non-empty TryRemove operations>
  %%  9  :  <#Empty TryRemove operations>
  %% 10-12: <matrix no.> <mm algorithm> <square matrix dimension N>
  %% 13-15: <work unit size> <phase 2 start time> <phase 3 start time>

  [t_rapl RAPL_PKG_power RAPL_CPU_power RAPL_UNCORE_power RAPL_DRAM_power] = \
    load_RAPL_power(basename, algname, casename);

  %printf("Case '%s', '%s', '%s'\n", basename, algname, casename);
  [basename algname '-' casename ' pinning=' sprintf("%d",res(3))]

  %% Prepare the output
  alg      = res(1);
  threads  = res(2);
  pinning  = res(3);
  matrix   = res(10);
  mmalg    = res(11);
  wus      = res(13);
  duration = res(4);
  phase1   = res(14) - res(6);
  phase2   = res(15) - res(14);
  phase3   = duration - (phase1 + phase2);
  durations = [duration phase1 phase2 phase3];
  operations = [res(7) res(8) res(9)];

  RAPL_t_avg = zeros(1, length(t_rapl));
  RAPL_t_avg_p1 = zeros(1, length(t_rapl));
  RAPL_t_avg_p2 = zeros(1, length(t_rapl));
  RAPL_t_avg_p3 = zeros(1, length(t_rapl));

  %% Select the active duration(s) for power measurement.
  %%   RAPL

  %% Full interval sample 1% to 99% of duration.
  if duration > 0.2
    RAPL_t_avg(lookup(t_rapl, res(6) + 0.01*duration)+1 : lookup(t_rapl, res(6) + 0.99*duration)) = 1;
  else
    %% Include at least two RAPL samples.
    RAPL_t_avg(lookup(t_rapl, res(6)) : lookup(t_rapl, res(6))+1) = 1;
  endif

  RAPL_t_avg = logical(RAPL_t_avg);

  if (res(14) > 0)
    %% Algorithm phases were timed.
    %% FIXME: Sample a subset of the full phase durations?
    RAPL_t_avg_p1(lookup(t_rapl, res(6))+1 : lookup(t_rapl, res(6)+phase1)) = 1;
    RAPL_t_avg_p1 = logical(RAPL_t_avg_p1);
    %% Phase 2 may be very short.
    if phase2 < 0.2
      %% Ignore phase 2 power.

      %%   Include one point from the end of phase 1 and one from the beginning
      %%   of phase 3.
      %RAPL_t_avg_p2(lookup(t_rapl, res(6)+phase1) : lookup(t_rapl, res(6)+phase1+phase2)+1) = 1;
    else
      RAPL_t_avg_p2(lookup(t_rapl, res(6)+phase1)+1 : lookup(t_rapl, res(6)+phase1+phase2)) = 1;
    endif
    RAPL_t_avg_p2 = logical(RAPL_t_avg_p2);
    RAPL_t_avg_p3(lookup(t_rapl, res(6)+phase1+phase2)+1 : lookup(t_rapl, res(6)+duration)) = 1;
    RAPL_t_avg_p3 = logical(RAPL_t_avg_p3);
  endif

  %% Average RAPL power over the selected intervals.
  RAPL_all = [RAPL_PKG_power RAPL_CPU_power RAPL_UNCORE_power RAPL_DRAM_power];

  RAPL_powers_mean = mean(RAPL_all(RAPL_t_avg,:))
  RAPL_powers_std = std(RAPL_all(RAPL_t_avg,:));
  RAPL_powers_biased_coef_of_var = RAPL_powers_std ./ RAPL_powers_mean

  if (res(14) > 0)
    %% Algorithm phases were timed.
    if phase1 > 0.2
      RAPL_powers_mean_p1 = mean(RAPL_all(RAPL_t_avg_p1,:))
      RAPL_powers_std_p1 = std(RAPL_all(RAPL_t_avg_p1,:));
      RAPL_powers_biased_coef_of_var_p1 = RAPL_powers_std_p1 ./ RAPL_powers_mean_p1
    else
      %% Ignore phase 1 power.
      RAPL_powers_mean_p1 = RAPL_powers_mean;
      RAPL_powers_std_p1  = RAPL_powers_std;
      RAPL_powers_biased_coef_of_var_p1 = RAPL_powers_biased_coef_of_var;
    endif

    if phase2 > 0.2
      RAPL_powers_mean_p2 = mean(RAPL_all(RAPL_t_avg_p2,:))
      RAPL_powers_std_p2  = std(RAPL_all(RAPL_t_avg_p2,:));
      RAPL_powers_biased_coef_of_var_p2 = RAPL_powers_std_p2 ./ RAPL_powers_mean_p2
    else
      %% Ignore phase 2 power.
      RAPL_powers_mean_p2 = RAPL_powers_mean;
      RAPL_powers_std_p2  = RAPL_powers_std;
      RAPL_powers_biased_coef_of_var_p2 = RAPL_powers_biased_coef_of_var;
    endif

    if phase3 > 0.2
      RAPL_powers_mean_p3 = mean(RAPL_all(RAPL_t_avg_p3,:))
      RAPL_powers_std_p3 = std(RAPL_all(RAPL_t_avg_p3,:));
      RAPL_powers_biased_coef_of_var_p3 = RAPL_powers_std_p3 ./ RAPL_powers_mean_p3
    else
      %% Ignore phase 3 power.
      RAPL_powers_mean_p3 = RAPL_powers_mean;
      RAPL_powers_std_p3  = RAPL_powers_std;
      RAPL_powers_biased_coef_of_var_p3 = RAPL_powers_biased_coef_of_var;
    endif

  else
    RAPL_powers_mean_p1 = RAPL_powers_mean;
    RAPL_powers_mean_p2 = RAPL_powers_mean;
    RAPL_powers_mean_p3 = RAPL_powers_mean;
    RAPL_powers_std_p1 = RAPL_powers_std;
    RAPL_powers_std_p2 = RAPL_powers_std;
    RAPL_powers_std_p3 = RAPL_powers_std;
    RAPL_powers_biased_coef_of_var_p1 = RAPL_powers_biased_coef_of_var;
    RAPL_powers_biased_coef_of_var_p2 = RAPL_powers_biased_coef_of_var;
    RAPL_powers_biased_coef_of_var_p3 = RAPL_powers_biased_coef_of_var;
  endif

  %% Set the RAPL output.
  RAPL_powers = [RAPL_powers_mean RAPL_powers_mean_p1 RAPL_powers_mean_p2 RAPL_powers_mean_p3];
  RAPL_powers_biased_coef_of_vars = [RAPL_powers_biased_coef_of_var RAPL_powers_biased_coef_of_var_p1 RAPL_powers_biased_coef_of_var_p2 RAPL_powers_biased_coef_of_var_p3];

  %% Optional RAPL power plot.
  if (plot_power)
    result = plot(t_rapl, RAPL_PKG_power(:,1),    "bo-;PKG S1 RAPL;",
                  t_rapl, RAPL_PKG_power(:,2),    "go-;PKG S2 RAPL;",
                  t_rapl, RAPL_CPU_power(:,1),    "b+-;CPU S1 RAPL;",
                  t_rapl, RAPL_CPU_power(:,2),    "g+-;CPU S2 RAPL;",
                  t_rapl, RAPL_UNCORE_power(:,1), "bx-;UNCORE S1 RAPL;",
                  t_rapl, RAPL_UNCORE_power(:,2), "gx-;UNCORE S2 RAPL;",
                  t_rapl, RAPL_DRAM_power(:,1),   "m+-;MEM S1 RAPL;",
                  t_rapl, RAPL_DRAM_power(:,2),   "c+-;MEM S2 RAPL;",
                  t_rapl, 69.*RAPL_t_avg,         "r-;Duration;",
                  t_rapl, 69.*RAPL_t_avg_p1,      "k-;Phase 1;",
                  t_rapl, 69.*RAPL_t_avg_p2,      "m-;Phase 2;",
                  t_rapl, 69.*RAPL_t_avg_p3,      "c-;Phase 3;"
                  );
    title(['RAPL power (' basename algname '-' casename ') pinning=' sprintf("%d",pinning)]);
    xlabel("Time instant (sec since the epoch)");
    ylim([0, 70]);
    ylabel("Power (W)");
    %print([basename algname '-' casename '-' sprintf("pin%d",pinning) '-power.eps'], "-deps","-color");
    sleep(3);
  endif

endfunction
