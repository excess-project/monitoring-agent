# Prepare PBS jobs for each number of threads.

# All threads
THREADS="2 4 6 8 10 12 14 16 18 20"
# Test max threads
#THREADS="20"
# All algorithms
ALGORITHMS="0 1 2 3 4 5 6 7 8 9 10 11 12"
# Just queues
#ALGORITHMS="0 1 2 3 4 5 6 7 8"
for T in $THREADS; do
    for I in $ALGORITHMS; do
        sed -e "s#CASETHREADS#$T#" -e "s#CASEALG#$I#" job > job.a$I.t$T
    done
done
