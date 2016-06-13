%% Instantiate the throughput model based on our IPDPS 2015 paper.
%%
%% Copyright (C) 2016  Anders Gidenstam, Chalmers University of Technology
%%

%% All work parameters are in cycles.

function params = EXCESS_IPDPS15_queue_model_instantiate(f, n, p_s, p_m, p_b, tp_d_ps_ps, tp_d_ps_pm, tp_d_pm_ps, tp_d_pm_pb, tp_e_ps_ps, tp_e_ps_pm, tp_e_pm_ps, tp_e_pm_pb, tp_e_pb_pm)

  %%     Low intra-contention case.
  %%       From paper
  if tp_d_pm_ps < tp_e_pm_ps

    hat_cw_dm = n .* f ./ tp_d_pm_ps - p_m;

    hat_cw_dp = (n .* f - (p_m + hat_cw_dm) .* tp_e_pm_pb) ./ (tp_d_pm_pb - tp_e_pm_pb) - p_m;
  else
    error("Failed to meet tp_d(p_m, p_s) < tp_e(p_m, p_s) for hat_cw_dm estimation.");
  endif
  %%       "In the same way as the above, using equations (1) and (3) and
  %%        (p_b,p_m) and (p_s,p_m)."
  if tp_e_ps_pm < tp_d_ps_pm

    hat_cw_ep = n .* f ./ tp_e_pb_pm - p_m;

    hat_cw_em = (n .* f - tp_d_ps_pm * p_s) ./ (tp_e_ps_pm - tp_d_ps_pm * p_s ./ (p_m + hat_cw_ep)) - p_m;

  else
    error("Failed to meet tp_e(p_s, p_m) < tp_d(p_s, p_m) for hat_cw_em estimation.");
  endif

  %%     High intra-contention case.
  %%       hat_Tp_dm / hat_Tp_dp
  if tp_d_ps_pm >= tp_e_ps_pm
    if tp_d_ps_ps >= tp_e_ps_ps

      hat_Tp_dm = (tp_e_ps_ps.*(tp_d_ps_pm - tp_e_ps_pm) - tp_e_ps_pm.*(tp_d_ps_ps - tp_e_ps_ps)) ./ (tp_d_ps_pm - tp_e_ps_pm - tp_d_ps_ps + tp_e_ps_ps);

    else
      hat_Tp_dm = tp_d_ps_ps;
    endif

    hat_Tp_dp = (tp_d_ps_pm - tp_e_ps_pm) ./ (1 - tp_e_ps_pm ./ hat_Tp_dm);
  else
    error("Failed to meet tp_d(idx_p_s, idx_p_m) >= tp_e(idx_p_s, idx_p_m)");
  endif

  %%       hat_Tp_em / hat_Tp_ep
  if tp_d_pm_ps >= tp_e_pm_ps
    if tp_d_ps_ps >= tp_e_ps_ps
      A = n.*freq - tp_d_ps_ps .* p_s;
      B = n.*freq - tp_d_pm_ps .* p_m;

      hat_Tp_ep = n.*f .* (A.*tp_e_pm_ps - B.*tp_e_ps_ps)./(A.*tp_d_pm_ps.*p_m - B.*tp_d_ps_ps.*p_s);
    else
      error("Failed to meet tp_d(idx_p_s, idx_p_s) >= tp_e(idx_p_s, idx_p_s)");
    endif
  else
    hat_Tp_ep = tp_e_pm_ps;
  endif

  if tp_d_ps_ps < tp_e_ps_ps
    hat_Tp_em = hat_Tp_ep;
  else
    hat_Tp_em = (n.*f .* tp_e_ps_ps - tp_d_ps_ps.*p_s.*hat_Tp_ep) ./ (n.*f - tp_d_ps_ps.*p_s);
  endif


  params = [ f n p_s hat_cw_dm hat_cw_dp hat_cw_em hat_cw_ep hat_Tp_dm hat_Tp_dp hat_Tp_em hat_Tp_ep ];
endfunction
