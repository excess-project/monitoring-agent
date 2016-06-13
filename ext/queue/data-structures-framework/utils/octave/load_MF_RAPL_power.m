%% Load energy/power measurements from RAPL recorded with the
%% EXCESS Monitoring Framework.
%% Anders Gidenstam  2015

function [t_rapl PKG_power CPU_power UNCORE_power DRAM_power] = load_MF_RAPL_power(basename)

  postfix = '.csv';
  rapl    = load([basename 'MFRAPL' postfix]);

  %% Times in seconds since the UNIX epoch for RAPL samples.
  t_rapl  = rapl(:,1);
  %% NOTE: A bug in the likwid plugin means the time between samples
  %%       for each socket is 2x (or 4x?) the actual measurement interval.
  dt_rapl = [diff(rapl(:,1)) diff(rapl(:,1))]./2;

  %% Process RAPL energy counts. (Joule between samples)
  %% Remove the first sample as we need the interval lengths.
  t_rapl = t_rapl(2:length(t_rapl),:);
  rapl = rapl(2:length(rapl),:);
  PKG_energy = [rapl(:,2) rapl(:,3)];
  CPU_energy =  [rapl(:,4) rapl(:,5)];
  DRAM_energy = [rapl(:,6) rapl(:,7)];

  %% Here UNCORE means whatever else contributes to the PKG energy.
  UNCORE_energy = PKG_energy - CPU_energy;

  %% Divide by the intersample interval here.
  PKG_power = PKG_energy./dt_rapl;
  CPU_power = CPU_energy./dt_rapl;
  DRAM_power = DRAM_energy./dt_rapl;
  UNCORE_power = UNCORE_energy./dt_rapl;

endfunction
