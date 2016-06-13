%% Summarize the result of a collection of Mandelbrot testbench cases.
%% Anders Gidenstam  2014 - 2015

FREQs = [3];
ALGs = [3];
%ALGS = 0:11;
THREADs = [10 20];
%THREADs = [2 4 6 8 10 12 14 16 18 20];
PATs = [0];
CONs = [0 1 2 3];
CONs = [3];

%% t20; a3; p0, p1, p2; c0 c1 c2 c3;
RUNs1 = [
         '/home/andersg/HLRS/results/mandelbrot_2015-01-30_11.14'
         '/home/andersg/HLRS/results/mandelbrot_2015-01-30_11.16'
         '/home/andersg/HLRS/results/mandelbrot_2015-01-30_11.18'
        ];

RUNs2 = [
         '/home/andersg/HLRS/results/mandelbrot_2015-01-30_13.13'
         '/home/andersg/HLRS/results/mandelbrot_2015-01-30_13.15'
         '/home/andersg/HLRS/results/mandelbrot_2015-01-30_13.18'
        ];

RUNs3 = [
         '/home/andersg/HLRS/results/mandelbrot_2015-01-30_13.44'
         '/home/andersg/HLRS/results/mandelbrot_2015-01-30_13.46'
         '/home/andersg/HLRS/results/mandelbrot_2015-01-30_13.48'
        ];

RUNs = RUNs3;

i = 1;
res = [];
for d = 1:size(RUNs)(1)
  base = RUNs(d,:);
  basename = [base '/mandelbrot_result_' base(length(base) - 15 : length(base)) '-'];

  for f = FREQs
    for a = ALGs

      algname = sprintf("a%d", a);

      for pat = PATs
        for con = CONs
          for t = THREADs
            casename = sprintf("p%d-c%d-f%d-t%d-1", pat, con, f, t);

            try
              resfile = sprintf("%sOUT-%s-%s.txt", basename, algname, casename);
              %printf("Trying '%s' ... ", outfile);

              [info, err] = stat(resfile);
              if (err == 0)
                [alg threads pinning pattern contention duration throughputs RAPL_powers] = summarize_mandelbrot_case(basename, algname, casename);
                %printf("succeeded\n");

                res(i,:) = [f alg threads pinning pattern contention duration throughputs RAPL_powers];
                i = i+1;
              else
                %printf("failed\n");
              endif
            catch
              printf("exception in summarize_mandelbrot case '%s'\n", resfile);
            end_try_catch
          endfor
        endfor
      endfor
    endfor
  endfor
endfor

csvwrite("result.res", res);
