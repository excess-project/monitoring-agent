# Submit PBS jobs.

THREADS="2 4 6 8 10 12 14 16 18 20"
ALGORITHMS="0 1 2 3 4 5 6 7 8 9 10 11"
for T in $THREADS; do
    for I in $ALGORITHMS; do
        qsub job.a$I.t$T
    done
done
