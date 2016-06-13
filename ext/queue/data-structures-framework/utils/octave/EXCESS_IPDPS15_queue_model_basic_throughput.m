%% Estimates average queue operation throughput based on our IPDPS 2015 paper.
%%
%% Copyright (C) 2015-2016  Anders Gidenstam, Chalmers University of Technology
%%
%% The parallel work is in cycles. See Section IV.D in the paper.
%%
function Tp_ob = EXCESS_IPDPS15_queue_model_basic_throughput(pw_o, params)

  f = params(1);
  n = params(2);
  p_s = params(3);
  hat_cw_ob = params(4);
  hat_Tp_ob = params(5);

  if pw_o <= (n - 1) .* hat_cw_ob
    Tp_ob = (f./hat_cw_ob - hat_Tp_ob) ./ ((n - 1).*hat_cw_ob - p_s) .* (pw_o - p_s) + hat_Tp_ob;
  else
    Tp_ob = n .*f ./ (pw_o + hat_cw_ob);
  endif
endfunction
