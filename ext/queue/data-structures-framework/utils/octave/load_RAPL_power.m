%% Load energy/power measurements from RAPL recorded with the state-record-tool.
%% NOTE: Configured for the likwid back-end. The PAPI back-end reports in nJ.
%% Anders Gidenstam  2014

function [t_rapl PKG_power CPU_power UNCORE_power DRAM_power] = load_RAPL_power(basename, algname, casename)

  postfix = ['-' algname '-' casename '.csv'];
  rapl    = load([basename 'RAPL' postfix]);

  %% Times in seconds since the UNIX epoch for RAPL samples.
  t_rapl  = rapl(2:length(rapl(:,1)),1);
  dt_rapl = [diff(rapl(:,1)) diff(rapl(:,1))];

  %% Process RAPL energy counts. (Joule)
  PKG_energy = [rapl(:,2) rapl(:,3)];
  DRAM_energy = [rapl(:,4) rapl(:,5)];
  CPU_energy =  [rapl(:,6) rapl(:,7)];

  %% Here UNCORE means whatever else contributes to the PKG energy.
  UNCORE_energy = PKG_energy - CPU_energy;

  %% Divide by the intersample interval here.
  PKG_power = diff(PKG_energy)./dt_rapl;
  CPU_power = diff(CPU_energy)./dt_rapl;
  DRAM_power = diff(DRAM_energy)./dt_rapl;
  UNCORE_power = diff(UNCORE_energy)./dt_rapl;

endfunction
