#!sh

N=$1
NTHREADS=$2
THETA=$3
RHO=$4

for i in std std_tcmalloc std_tbb tbb_scalable tbb_allocator
do
    OFILE=$i.$NTHREADS.N$N.t$THETA.r$RHO.time.txt
    if [ ! -s $OFILE ]
    then
	/usr/bin/time -f "%e %M" -o $OFILE ./$i $N $NTHREADS $THETA $RHO
    fi
done
