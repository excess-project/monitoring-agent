%% Load the result (output of the testbench program) for one case.
%% Anders Gidenstam  2014

function result = load_case_result(basename, algname, casename)

  postfix = ['-' algname '-' casename '.txt'];
  result  = load([basename 'OUT' postfix]);

endfunction
