%% Evaluation of variant selection for various queue performance prediction
%% methods
%%
%% Copyright (C) 2016  Anders Gidenstam, Chalmers University of Technology
%%

%% Select cases
threads=18;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% The raw data.
res_empirical = load('producerconsumer_2016-01-05_09.51--2016-01-09_10.10.res');
%% The raw data.
%% Layout of empirical result file:
%%   1-7:  freq alg threads pinning pattern ppw cpw
%%   8-10: tp_insert tp_try_remove_ok tp_try_remove_empty
%%  11-14: P_PKG_S1 P_PKG_S2 P_CPU_S1 P_CPU_S2
%%  15-18: P_Uncore_S1 P_Uncore_S2 P_Mem_S1 P_Mem_S2
%%  19-22: BCoV_PKG_S1 BCoV_PKG_S2 BCoV_CPU_S1 BCoV_CPU_S2
%%  23-26: BCoV_Uncore_S1 BCoV_Uncore_S2 BCoV_Mem_S1 BCoV_Mem_S2

%% Extract the domain ranges.
E_range_freq    = unique(res_empirical(:,1));
E_range_impl    = unique(res_empirical(:,2));
E_range_threads = unique(res_empirical(:,3));
E_range_pinning = unique(res_empirical(:,4));
E_range_pattern = unique(res_empirical(:,5));
E_range_pw_e    = unique(res_empirical(:,6));
E_range_pw_d    = unique(res_empirical(:,7));

E_res_t = res_empirical(res_empirical(:,3)==threads,:);

%% Extract "best" impl from the raw empirical data.
E_best_t = -1.*ones(rows(E_range_pw_d), rows(E_range_pw_e));
for idx_d = 1:rows(E_range_pw_d)
  E_res_td = E_res_t(E_res_t(:,7)==E_range_pw_d(idx_d),:);
  for idx_e = 1:rows(E_range_pw_e)
    E_res_tde = E_res_td(E_res_td(:,6)==E_range_pw_e(idx_e),:);
    best_eff=0;
    for idx_impl = 1:rows(E_range_impl)
      E_res_tdei = E_res_tde(E_res_tde(:,2)==E_range_impl(idx_impl),:);
      if size(E_res_tdei)(1) == 1
        throughput = min(E_res_tdei(1,8), sum(E_res_tdei(1,9:10)));
        power      = sum([E_res_tdei(1,11:12) E_res_tdei(1,17:18)]);
        eff = throughput/power;
        if eff > best_eff
          best_eff = eff;
          E_best_t(idx_d, idx_e) = E_range_impl(idx_impl);
        endif
      endif
    endfor
  endfor
endfor

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Instantiate one EXCESS IPDPS 2015 type model per implementation
%% (and number of threads)
%%
%% Model instantiation
%%
%%   Parameters
%%     #enqueuers, #dequeuers
n = threads./2; % For the uniform #enqueuers/#dequeuers case.
%%     pw_o data points for the model instantiation. The model uses 6 points:
%%       (p_s,p_s), (p_s,p_m), (p_m,p_s), (p_m,p_b), (p_b,p_m), (p_b,p_b)
p_s = 5;
p_m = 150;
p_b = 1000;
idx_p_s = lookup(E_range_pw_e, p_s);
idx_p_m = lookup(E_range_pw_e, p_m);
idx_p_b = lookup(E_range_pw_e, p_b);

model = -1.*ones(rows(E_range_impl),11);
%cycles_per_wu = zeros(rows(E_range_impl),1);

for idx_impl = 1:rows(E_range_impl)
  try
    %% Extract the relevant empirical data
    E_res_ti = E_res_t(E_res_t(:,2)==E_range_impl(idx_impl),:);

    %% Collect the throughputs over the pw_d x pw_e domain
    tp_e  = zeros(rows(E_range_pw_d), rows(E_range_pw_e));
    tp_dp = zeros(rows(E_range_pw_d), rows(E_range_pw_e));
    tp_dm = zeros(rows(E_range_pw_d), rows(E_range_pw_e));

    for idx_d = 1:rows(E_range_pw_d)
      E_res_tid = E_res_ti(E_res_ti(:,7)==E_range_pw_d(idx_d),:);
      for idx_e = 1:rows(E_range_pw_e)
        tp_e(idx_d, idx_e)  = E_res_tid(E_res_tid(:,6)==E_range_pw_e(idx_e), 8);
        tp_dp(idx_d, idx_e) = E_res_tid(E_res_tid(:,6)==E_range_pw_e(idx_e), 9);
        tp_dm(idx_d, idx_e) = E_res_tid(E_res_tid(:,6)==E_range_pw_e(idx_e), 10);
      endfor
    endfor
    tp_d = tp_dp + tp_dm;

    %%     "Unit of work" conversion to cycles
    cycles_per_wu(idx_impl) = n .* E_range_freq(1)*1e9 ./ (tp_e(idx_p_b, idx_p_b) .* p_b);

    %% Instantiate the queue model.
    model(idx_impl,:) = EXCESS_IPDPS15_queue_model_instantiate(E_range_freq(1)*1e9, n, p_s*cycles_per_wu(idx_impl), p_m*cycles_per_wu(idx_impl), p_b*cycles_per_wu(idx_impl), tp_d(idx_p_s,idx_p_s), tp_d(idx_p_s,idx_p_m), tp_d(idx_p_m,idx_p_s), tp_d(idx_p_m,idx_p_b), tp_e(idx_p_s,idx_p_s), tp_e(idx_p_s,idx_p_m), tp_e(idx_p_m,idx_p_s), tp_e(idx_p_m,idx_p_b), tp_e(idx_p_b,idx_p_m));

  catch
    printf("Failed to instantiate EXCESS_IPDPS15_queue_model for implementation %d.\n", E_range_impl(idx_impl));
  end_try_catch
endfor

%% Extract "best" impl from the instantiated models.
%% NOTE: Since the power model is not yet available efficiency here is just throughput.
%%       This is equivalent to using a simpler power model that doesn't depend on the
%%       implementation.
M_best_t = -1.*ones(rows(E_range_pw_d), rows(E_range_pw_e));
for idx_d = 1:rows(E_range_pw_d)
  for idx_e = 1:rows(E_range_pw_e)
    best_eff=0;
    for idx_impl = 1:rows(E_range_impl)
      if model(idx_impl,1) > 0
        [Tp_d Tp_e Tp_dp Tp_dm Tp_ep Tp_em] = EXCESS_IPDPS15_queue_model_throughput(cycles_per_wu(idx_impl)*E_range_pw_d(idx_d), cycles_per_wu(idx_impl)*E_range_pw_e(idx_e), model(idx_impl,:));

        eff = min([Tp_e Tp_d]);
        if eff > best_eff
          best_eff = eff;
          M_best_t(idx_d, idx_e) = E_range_impl(idx_impl);
        endif
      endif
    endfor
  endfor
endfor

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Results from the AdaptiveSampling implementation by Kim Strömberg.
%% The result consists of the cases sampled by the algorithm.
res_adaptive  = load('AdaptiveSampling-producerconsumer_2016-01-05_09.51--2016-01-09_10.10.res');
%% Layout of adaptive result file:
%%   1-6:  freq threads pinning pattern ppw cpw
%%   7-9:  best_alg throughput power

%% Extract the domain ranges.
A_range_freq    = unique(res_adaptive(:,1));
A_range_impl    = unique(res_adaptive(:,7));
A_range_threads = unique(res_adaptive(:,2));
A_range_pinning = unique(res_adaptive(:,3));
A_range_pattern = unique(res_adaptive(:,4));
A_range_pw_e    = unique(res_adaptive(:,5));
A_range_pw_d    = unique(res_adaptive(:,6));

%% Reformat the best impl from the Adaptive sampler to a pw_d x pw_e matrix.
%% Cells that retain the value -1 has not been sampled.
A_res_t = res_adaptive(res_adaptive(:,2)==threads,:);
A_best_t = -1.*ones(rows(A_range_pw_d), rows(A_range_pw_e));
for idx_d = 1:rows(A_range_pw_d)
  A_res_td = A_res_t(A_res_t(:,6)==A_range_pw_d(idx_d),:);
  for idx_e = 1:rows(A_range_pw_e)
    A_res_tde = A_res_td(A_res_td(:,5)==A_range_pw_e(idx_e),:);
    if size(A_res_tde)(1) == 1
      A_best_t(idx_d, idx_e) = A_res_tde(:,7);
    endif;
  endfor
endfor

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Print the best implementation over the pw_d x pw_e domain at a certain
%% number of threads.
printf("Best implementation over the pw_d x pw_e domain:\n");
threads
Empirical_best_t = E_best_t

p_s
p_m
p_b
IPDPS15_model_best_t = M_best_t
error = length(nonzeros(M_best_t - E_best_t))./(rows(E_range_pw_d)*rows(E_range_pw_e))

AdaptiveSampling_best_t = A_best_t


%% Plot best impl at N threads over cw_d and cw_e.
if 0

color = ['r', 'g', 'b', 'c', 'm', 'k', 'r', 'g', 'b', 'c', 'm', 'k', 'y'];
color  = [color color];
symb  = ['+', 'x', 'o', '*', 's', 'd', '^', 'v', '<', '>', 'p', 'h'];
symb  = [symb symb];
for impl=0:12
  E_best_ti = E_best_t(E_best_t(:,2)==impl,:);
  plot(E_best_ti(:,6), E_best_ti(:,7), [color(impl+1) symb(impl+1) ";Impl. " num2str(impl) ";"]);
  hold on;
endfor

title("Best Producer/consumer collection");
xlabel("Enqueuer parallel work (wu)");
ylabel("Dequeuer parallel work (wu)");

hold off;

endif
