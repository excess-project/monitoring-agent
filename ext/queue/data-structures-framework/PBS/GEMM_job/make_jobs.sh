# Prepare PBS jobs for each number of threads.

THREADS="2 4 6 8 10 12 14 16 18 20"
for T in $THREADS; do
    sed -e "s#CASETHREADS#$T#" job > job.t$T
done
