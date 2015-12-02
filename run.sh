#!sh

for i in std std_tcmalloc std_tbb tbb_scalable tbb_allocator
do
    if [ ! -s $i.$1.time.txt ]
    then
	/usr/bin/time -f "%e %M" -o $i.$1.time.txt ./$i 10000 $1 500 500
    fi
done
