%% Estimates average queue operation throughput based on our IPDPS 2015 paper.
%%
%% Copyright (C) 2015-2016  Anders Gidenstam, Chalmers University of Technology
%%
%% The parallel work is in cycles.
%%

function [Tp_d Tp_e Tp_dp Tp_dm Tp_ep Tp_em] = EXCESS_IPDPS15_queue_model_throughput(pw_d, pw_e, params)

  f = params(1);
  n = params(2);
  p_s = params(3);
  hat_cw_dm = params(4);
  hat_cw_dp = params(5);
  hat_cw_em = params(6);
  hat_cw_ep = params(7);
  hat_Tp_dm = params(8);
  hat_Tp_dp = params(9);
  hat_Tp_em = params(10);
  hat_Tp_ep = params(11);

  %% Compute the basic throughputs.
  Tp_dm = EXCESS_IPDPS15_queue_model_basic_throughput(pw_d, [f n p_s hat_cw_dm hat_Tp_dm]);
  Tp_dp = EXCESS_IPDPS15_queue_model_basic_throughput(pw_d, [f n p_s hat_cw_dp hat_Tp_dp]);
  Tp_em = EXCESS_IPDPS15_queue_model_basic_throughput(pw_e, [f n p_s hat_cw_em hat_Tp_em]);
  Tp_ep = EXCESS_IPDPS15_queue_model_basic_throughput(pw_e, [f n p_s hat_cw_ep hat_Tp_ep]);

  %% Make sure Tp_op is not less than Tp_om.
  if 1
    Tp_dp = max(Tp_dp, Tp_dm);
    Tp_ep = max(Tp_ep, Tp_em);
  else
    Tp_dm = min(Tp_dp, Tp_dm);
    Tp_em = min(Tp_ep, Tp_em);
  endif

  %% Combine the basic throughputs. See Section IV.C in the paper.
  if Tp_em ./ Tp_dm <= 1 - pw_d./(n.*f) .* (Tp_ep - Tp_em)
    %% Case 2: Mostly empty queue.
    %%   From (2) and (3):
    Tp_d = (Tp_dp + Tp_em.*(1 - Tp_dp./Tp_dm)) ./ (1 - pw_d./(n.*f) .* (Tp_ep - Tp_em) .* (1 - Tp_dp./Tp_dm));
    Tp_e = Tp_d .* pw_d ./ (n.*f) .* Tp_ep + (1 - Tp_d.*pw_d ./ (n.*f)) .* Tp_em;

    if Tp_ep > Tp_dm
      %% Case 1 also applies. Take mean.
      Tp_d = mean([Tp_d  Tp_dm]);
      Tp_e = mean([Tp_e  Tp_ep]);
    endif
  else
    %% Case 2 doesn't apply.
    if Tp_ep > Tp_dm
      %% Case 1: Growing queue
      Tp_d = Tp_dm;
      Tp_e = Tp_ep;
    else
      %% Should never happen.
      error("Bad case in throughput prediction! The model instantiation is probably faulty.");
      %% But if it does!?
      Tp_d = 0;
      Tp_e = 0;
    endif
  endif
endfunction
