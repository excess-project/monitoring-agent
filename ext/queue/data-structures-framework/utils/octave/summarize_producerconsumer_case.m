%% Summarize the result of one Producer-Consumer testbench case.
%% Anders Gidenstam  2014 - 2015

function [alg threads pinning pattern pcpw throughputs RAPL_powers RAPL_powers_biased_coef_of_var] = summarize_producerconsumer_case(basename, algname, casename, plot_power)

  res = load_case_result(basename, algname, casename);
  [t_rapl RAPL_PKG_power RAPL_CPU_power RAPL_UNCORE_power RAPL_DRAM_power] = \
    load_RAPL_power(basename, algname, casename);

  %printf("Case '%s', '%s', '%s'\n", basename, algname, casename);
  [basename algname '-' casename ' pinning=' sprintf("%d",res(3))]

  %% Prepare the output
  alg     = res(1);
  threads = res(2);
  pinning = res(3);

  RAPL_t_avg = zeros(1, length(t_rapl));
  %% Distinguish between standard and phased mode via the length of res.
  if (length(res) <= 12)
    %% Standard mode
    %%   Duration
    duration = res(4);

    %% Select the active duration for power measurement.
    %% Currently the middle 50% of the active interval.
    %%   RAPL
    RAPL_t_avg(lookup(t_rapl, res(6) + 0.25*duration)+1 : lookup(t_rapl, res(6) + 0.75*duration)) = 1;

  else
    %% Phased mode
    %%   Duration
    duration = res(14);
    phases = res(15)
    active_fraction = duration/res(4)

    %% Select the active phases for power measurement.
    %%   RAPL
    pmax = min(phases,15);
    i = 0;
    while i < pmax
      end_idx = lookup(t_rapl, res(end - i));
      start_idx = min(lookup(t_rapl, res(end - i - 1)) + 2, end_idx);
      RAPL_t_avg(start_idx:end_idx) = 1;
      i = i+2;
    endwhile
  endif
  RAPL_t_avg = logical(RAPL_t_avg);

  pattern = res(10);
  pcpw    = res(11:12);
  throughputs = res(7:9)./duration

  %% Average RAPL power over the selected intervals.
  RAPL_all = [RAPL_PKG_power RAPL_CPU_power RAPL_UNCORE_power RAPL_DRAM_power];

  RAPL_powers_mean = mean(RAPL_all(RAPL_t_avg,:))
  RAPL_powers_std = std(RAPL_all(RAPL_t_avg,:));
  RAPL_powers_biased_coef_of_var = RAPL_powers_std ./ RAPL_powers_mean

  %% Set the RAPL output.
  RAPL_powers = RAPL_powers_mean;

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
                  t_rapl, 69.*RAPL_t_avg,         "r-;Averaging window;"
                  );
    title(['RAPL power (' basename algname '-' casename ') pinning=' sprintf("%d",pinning)]);
    xlabel("Time instant (sec since the epoch)");
    ylim([0, 70]);
    ylabel("Power (W)");
    %print([basename algname '-' casename '-' sprintf("pin%d",pinning) '-power.eps'], "-deps","-color");
    sleep(3);
  endif

endfunction
