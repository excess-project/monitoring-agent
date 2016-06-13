%% Load power measurements for the whole cluster from the external DAQ
%% recorded with the EXCESS Monitoring Framework.
%% Anders Gidenstam  2015

function [t_daq NODE01_power NODE02_power NODE03_power NAS_power] = load_MF_cluster_power(basename)

  postfix = '.csv';
  daq     = load([basename 'MFCLUSTER' postfix]);

  %% Times in seconds since the UNIX epoch for the samples.
  t_daq  = daq(:,1);

  NODE01_power = daq(:,2);
  NODE02_power = daq(:,3);
  NODE03_power = daq(:,4);
  NAS_power    = daq(:,5);

endfunction
