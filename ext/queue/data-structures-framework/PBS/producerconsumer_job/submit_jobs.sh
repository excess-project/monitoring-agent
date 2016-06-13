# Submit PBS jobs for each number of threads.

# All threads
#THREADS="2 4 6 8 10 12 14 16 18 20"
# Test max threads
THREADS="20"
# All algorithms
ALGORITHMS="0 1 2 3 4 5 6 7 8 9 10 11 12"
# Just queues
#ALGORITHMS="0 1 2 3 4 5 6 7 8"
# Just Michael-Scott?
#ALGORITHMS="0"
PATTERNS="2 3 4 5 7 8 9 26 27 28"
for I in $ALGORITHMS; do
    for T in $THREADS; do
        for P in $PATTERNS; do
            qsub job.a$I.t$T.p$P
        #./qsub.sh job.a$I.t$T
        done
    done
done
