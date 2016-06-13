%% Estimates average queue operation power and throughput based on our
%% IPDPS 2015 paper.
%%
%% Copyright (C) 2015-2016  Anders Gidenstam, Chalmers University of Technology
%%

%% Source data.
%fulldomain_all = load("/home/andersg/HLRS/results/producerconsumer_2015-11-23_08.44--2015-11-25_00.54.res");
fulldomain_all = load("/home/andersg/HLRS/results/producerconsumer_2016-01-05_09.51--2016-01-09_10.10.res");

%% Layout of result file:
%%   1-7:  freq alg threads pinning pattern ppw cpw
%%   8-10: tp_insert tp_try_remove_ok tp_try_remove_empty
%%  11-14: P_PKG_S1 P_PKG_S2 P_CPU_S1 P_CPU_S2
%%  15-18: P_Uncore_S1 P_Uncore_S2 P_Mem_S1 P_Mem_S2
%%  19-22: BCoV_PKG_S1 BCoV_PKG_S2 BCoV_CPU_S1 BCoV_CPU_S2
%%  23-26: BCoV_Uncore_S1 BCoV_Uncore_S2 BCoV_Mem_S1 BCoV_Mem_S2

%% The algorithms
%%    0. Queue NOBLE Lock-Free DB
%%    1. Queue NOBLE Lock-Free DU
%%    2. Queue NOBLE Lock-Free SB
%%    3. Queue NOBLE Lock-Free BB
%%    4. Queue NOBLE Lock-Free BASKET
%%    5. Queue NOBLE Lock-Free ELIM
%%    6. Queue NOBLE Lock-Based
%%    7. Queue TBB concurrent_queue
%%    8. Queue M&S two-lock queue
%%    9. Stack NOBLE Lock-Free B
%%   10. Stack NOBLE Lock-Free ELIM
%%   11. Bag NOBLE Lock-Free BB
%%   12. Pool NOBLE Lock-Free EDTREE
alg = 8;
threads=10;
freq=3.0e9;

%%
%% Model instantiation parameters
%%   pw_o data points for the model instantiation
%%   R-file: es=10; em=50; db=1000; ds=1; dm=150; eb=1000;
p_s = 1;
p_m = 50;
p_b = 1000;


%% Extract the raw data
fulldomain1 = fulldomain_all(fulldomain_all(:,1)==freq./1e9,:);
fulldomain2 = fulldomain_all(fulldomain1(:,2)==alg,:);
fulldomain = fulldomain2(fulldomain2(:,3)==threads,:);

fullpw_e = unique(fulldomain(:,6));
fullpw_d = unique(fulldomain(:,7));

%% Collect the throughputs for the whole domain
tp_e  = zeros(rows(fullpw_d), rows(fullpw_e));
tp_dp = zeros(rows(fullpw_d), rows(fullpw_e));
tp_dm = zeros(rows(fullpw_d), rows(fullpw_e));

for idx_e = 1:rows(fullpw_e)
  domain_e = fulldomain(fulldomain(:,6)==fullpw_e(idx_e),:);
  for idx_d = 1:rows(fullpw_d)
    tp_e(idx_d, idx_e)  = domain_e(domain_e(:,7)==fullpw_d(idx_d),8);
    tp_dp(idx_d, idx_e) = domain_e(domain_e(:,7)==fullpw_d(idx_d),9);
    tp_dm(idx_d, idx_e) = domain_e(domain_e(:,7)==fullpw_d(idx_d),10);
  endfor
endfor
tp_d = tp_dp + tp_dm;

%%
%% Model instantiation
%%
%%   Parameters
%%     #enqueuers, #dequeuers
n = threads./2; % For the uniform #enqueuers/#dequeuers case.
n_e = threads./2;
n_d = threads./2;
%%     pw_o data points for the model instantiation
%%     R-file: es=10; em=50; db=1000; ds=1; dm=150; eb=1000;
%p_s = 5;
%p_m = 150;
%p_b = 1000;
idx_p_s = lookup(fullpw_e, p_s);
idx_p_m = lookup(fullpw_e, p_m);
idx_p_b = lookup(fullpw_e, p_b);

%%     "Unit of work" conversion to cycles
cycles_per_wu = n .* freq ./ (tp_e(idx_p_b, idx_p_b) .* p_b);

%%     Instantiate the queue model.
%%     NOTE: The data point entry is failure prone.
params = EXCESS_IPDPS15_queue_model_instantiate(freq, n, p_s*cycles_per_wu, p_m*cycles_per_wu, p_b*cycles_per_wu, tp_d(idx_p_s,idx_p_s), tp_d(idx_p_s,idx_p_m), tp_d(idx_p_m,idx_p_s), tp_d(idx_p_m,idx_p_b), tp_e(idx_p_s,idx_p_s), tp_e(idx_p_s,idx_p_m), tp_e(idx_p_m,idx_p_s), tp_e(idx_p_m,idx_p_b), tp_e(idx_p_b,idx_p_m));

%% Print all model instantiation parameters
if 1
  alg
  freq
  cycles_per_wu
  n
  p_s
  p_m
  p_b
  hat_cw_dm = params(4)
  hat_cw_dp = params(5)
  hat_cw_em = params(6)
  hat_cw_ep = params(7)
  hat_Tp_dm = params(8)
  hat_Tp_dp = params(9)
  hat_Tp_em = params(10)
  hat_Tp_ep = params(11)
endif

%% Predict the full domain
densepw_o = union(fullpw_d, [[0.25 0.5] [1:1:10] [10:5:50] [50:50:fullpw_d(rows(fullpw_d))]])';
ETp_e = zeros(rows(densepw_o), rows(densepw_o));
ETp_d = zeros(rows(densepw_o), rows(densepw_o));

ETp_ep = zeros(rows(densepw_o), rows(densepw_o));
ETp_em = zeros(rows(densepw_o), rows(densepw_o));

ETp_dp = zeros(rows(densepw_o), rows(densepw_o));
ETp_dm = zeros(rows(densepw_o), rows(densepw_o));

for idx_e = 1:rows(densepw_o)
  for idx_d = 1:rows(densepw_o)
    [mptp_d mptp_e mptp_dp mptp_dm mptp_ep mptp_em] = EXCESS_IPDPS15_queue_model_throughput(densepw_o(idx_d).*cycles_per_wu, densepw_o(idx_e).*cycles_per_wu, params);

    ETp_e(idx_d, idx_e) = mptp_e;
    ETp_d(idx_d, idx_e) = mptp_d;

    %% These are the same for all pw levels of the other operation.
    ETp_ep(idx_d, idx_e) = mptp_ep;
    ETp_em(idx_d, idx_e) = mptp_em;

    ETp_dp(idx_d, idx_e) = mptp_dp;
    ETp_dm(idx_d, idx_e) = mptp_dm;

  endfor
endfor

%%
%% Plots
%%

if 0
%% 3d plot of throughputs v.s. parallel work
%% Use 3d points for the real data?
hold on;
if 0
  %% Raw data
  %%   Linear scale
  x = fullpw_e;
  y = fullpw_d;
  mesh(x, y, tp_e);
  mesh(x, y, tp_d);
  %mesh(x, y, tp_dp);
  %mesh(x, y, tp_dm);
  %legend("tp_e", "tp_{dp}", "tp_{dm}");
  legend("tp_e", "tp_{d}");
  %% Prediction
  [xx, yy] = meshgrid(densepw_o, densepw_o);
  plot3(xx, yy, ETp_e,  "o");
  plot3(xx, yy, ETp_d, "+");
  %mesh(x, y, ETp_e);
  %mesh(x, y, ETp_d);
else
  %% Prediction
  %%   Linear scale
  mx = densepw_o;
  my = densepw_o;
  mesh(mx, my, ETp_e);
  mesh(mx, my, ETp_d);
  legend("ETp_e", "ETp_{d}");

  %% Raw data
  if 1
    [rxx, ryy] = meshgrid (fullpw_e, fullpw_d);
    plot3(rxx, ryy, tp_e, "+");
    plot3(rxx, ryy, tp_d, "o");
    %plot3(xx, yy, tp_dp, "o");
    %plot3(xx, yy, tp_dm, "x");
  else
    rx = fullpw_e;
    ry = fullpw_d;
    mesh(rx, ry, tp_e);
    mesh(rx, ry, tp_d);
    %mesh(rx, ry, tp_dp);
    %mesh(rx, ry, tp_dm);
  endif
endif

title("Producer/consumer collection throughputs");
xlabel("Enqueuer parallel work");
ylabel("Dequeuer parallel work");
zlabel("Throughput (ops/sec)");
hold off;

else
%% 2D plots

pn = 1;
p_1 = p_s;
p_2 = p_m;
p_3 = p_b;

%%   Enqueuer throughput
figure(pn++);
str_pw_d = [" (pw_d = " num2str(p_1) ")"];
semilogx(
  fullpw_e,  tp_e(lookup(fullpw_d, p_1),:),   ["ys;tp_e" str_pw_d ";"],
  densepw_o, ETp_e(lookup(densepw_o, p_1),:), [":y-;ETp_e" str_pw_d ";"]);
hold on;
str_pw_d = [" (pw_d = " num2str(p_2) ")"];
semilogx(
  fullpw_e,  tp_e(lookup(fullpw_d, p_2),:),   ["cs;tp_e" str_pw_d ";"],
  densepw_o, ETp_e(lookup(densepw_o, p_2),:), [":c-;ETp_e" str_pw_d ";"]);
str_pw_d = [" (pw_d = " num2str(p_3) ")"];
semilogx(
  fullpw_e,  tp_e(lookup(fullpw_d, p_3),:),   ["ms;tp_e" str_pw_d ";"],
  densepw_o, ETp_e(lookup(densepw_o, p_3),:), [":m-;ETp_e" str_pw_d ";"]);

semilogx(
  densepw_o, ETp_ep(lookup(densepw_o, p_1),:), ["k^;ETp_{ep};"],
  densepw_o, ETp_em(lookup(densepw_o, p_1),:), ["kv;ETp_{em};"]);

str_pw_d = [" (pw_d = " num2str(p_1) ")"];
semilogx(
  fullpw_e,  tp_d(lookup(fullpw_d, p_1),:),    ["ro;tp_d" str_pw_d ";"],
  densepw_o, ETp_d(lookup(densepw_o, p_1),:),  ["r;ETp_d" str_pw_d ";"]);
str_pw_d = [" (pw_d = " num2str(p_2) ")"];
semilogx(
  fullpw_e,  tp_d(lookup(fullpw_d, p_2),:),    ["go;tp_d" str_pw_d ";"],
  densepw_o, ETp_d(lookup(densepw_o, p_2),:),  ["g;ETp_d" str_pw_d ";"]);
str_pw_d = [" (pw_d = " num2str(p_3) ")"];
semilogx(
  fullpw_e,  tp_d(lookup(fullpw_d, p_3),:),    ["bo;tp_d" str_pw_d ";"],
  densepw_o, ETp_d(lookup(densepw_o, p_3),:),  ["b;ETp_d" str_pw_d ";"]);
%semilogx(
%  densepw_o, ETp_dp(lookup(densepw_o, p_1),:), ["k^;ETp_{dp};"],
%  densepw_o, ETp_dm(lookup(densepw_o, p_1),:), ["kv;ETp_{dm};"]);

title(["Throughput.v.s. enqueuer parallel work for alg. " num2str(alg) " at " num2str(2.*n) " threads."]);
xlabel("Enqueuer parallel work (wu)");
ylabel("Enqueuer throughput (ops/sec)");
hold off;

%%   Throughput v.s. dequeuer parallel work
figure(pn++);
str_pw_e = [" (pw_e = " num2str(p_1) ")"];
semilogx(
  fullpw_d,  tp_e(:,lookup(fullpw_e, p_1)),    ["ys;tp_e" str_pw_e ";"],
  densepw_o, ETp_e(:,lookup(densepw_o, p_1)),  [":y;ETp_e" str_pw_e ";"]);
hold on;
str_pw_e = [" (pw_e = " num2str(p_2) ")"];
semilogx(
  fullpw_d,  tp_e(:,lookup(fullpw_e, p_2)),    ["cs;tp_e" str_pw_e ";"],
  densepw_o, ETp_e(:,lookup(densepw_o, p_2)),  [":c;ETp_e" str_pw_e ";"]);
str_pw_e = [" (pw_e = " num2str(p_3) ")"];
semilogx(
  fullpw_d,  tp_e(:,lookup(fullpw_e, p_3)),    ["ms;tp_e" str_pw_e ";"],
  densepw_o, ETp_e(:,lookup(densepw_o, p_3)),  [":m;ETp_e" str_pw_e ";"]);
%semilogx(
%  densepw_o, ETp_ep(:,lookup(densepw_o, p_1),:), ["k^;ETp_{ep};"],
%  densepw_o, ETp_em(:,lookup(densepw_o, p_1),:), ["kv;ETp_{em};"]);

str_pw_e = [" (pw_e = " num2str(p_1) ")"];
semilogx(
  fullpw_d,  tp_d(:,lookup(fullpw_e, p_1)),    ["ro;tp_d" str_pw_e ";"],
  densepw_o, ETp_d(:,lookup(densepw_o, p_1)),  ["r-;ETp_d" str_pw_e ";"]);
semilogx(
  fullpw_d,  tp_dp(:,lookup(fullpw_e, p_1)),   ["r+;tp_{dp}" str_pw_e ";"]);
semilogx(
  fullpw_d,  tp_dm(:,lookup(fullpw_e, p_1)),   ["rx;tp_{dm}" str_pw_e ";"]);

str_pw_e = [" (pw_e = " num2str(p_2) ")"];
semilogx(
  fullpw_d,  tp_d(:,lookup(fullpw_e, p_2)),    ["go;tp_d" str_pw_e ";"],
  densepw_o, ETp_d(:,lookup(densepw_o, p_2)),  ["g-;ETp_d" str_pw_e ";"]);
semilogx(
  fullpw_d,  tp_dp(:,lookup(fullpw_e, p_2)),   ["g+;tp_{dp}" str_pw_e ";"]);
semilogx(
  fullpw_d,  tp_dm(:,lookup(fullpw_e, p_2)),   ["gx;tp_{dm}" str_pw_e ";"]);

str_pw_e = [" (pw_e = " num2str(p_3) ")"];
semilogx(
  fullpw_d,  tp_d(:,lookup(fullpw_e, p_3)),    ["bo;tp_d" str_pw_e ";"],
  densepw_o, ETp_d(:,lookup(densepw_o, p_3)),  ["b-;ETp_d" str_pw_e ";"]);
semilogx(
  fullpw_d,  tp_dp(:,lookup(fullpw_e, p_3)),   ["b+;tp_{dp}" str_pw_e ";"]);
semilogx(
  fullpw_d,  tp_dm(:,lookup(fullpw_e, p_3)),   ["bx;tp_{dm}" str_pw_e ";"]);

semilogx(
  densepw_o, ETp_dp(:,lookup(densepw_o, p_1)), ["k^;ETp_{dp};"],
  densepw_o, ETp_dm(:,lookup(densepw_o, p_1)), ["kv;ETp_{dm};"]);

title(["Throughput.v.s. dequeuer parallel work for alg. " num2str(alg) " at " num2str(2.*n) " threads."]);
xlabel("Dequeuer parallel work (wu)");
ylabel("Throughput (ops/sec)");
hold off;

endif
