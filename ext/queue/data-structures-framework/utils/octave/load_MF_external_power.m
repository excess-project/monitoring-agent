%% Load power measurements for one node from the external DAQ recorded with the
%% EXCESS Monitoring Framework.
%% Anders Gidenstam  2015

function [t_daq PKG_power ATX12V_power GPU0_power] = load_MF_external_power(basename)

  postfix = '.csv';
  daq     = load([basename 'MFEXTERNAL' postfix]);

  %% Times in seconds since the UNIX epoch for the samples.
  t_daq  = daq(:,1);

  PKG_power    = daq(:,2:3);
  ATX12V_power = daq(:,4);
  GPU0_power   = daq(:,5);

endfunction
