#!sh

N=$1
NTHREADS=$2
THETA=$3
RHO=$4

#Find all executables in cwd, assume they are examples, and run them
for i in $(find . -maxdepth 1 -executable -type f)
do
    OFILE=$i.$NTHREADS.N$N.t$THETA.r$RHO.time.txt
    if [ ! -s $OFILE ]
    then
	/usr/bin/time -f "%e %M" -o $OFILE ./$i $N $NTHREADS $THETA $RHO
    fi
done
