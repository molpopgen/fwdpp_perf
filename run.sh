#!sh

for i in std std_tcmalloc std_tbb
do
    /usr/bin/time -f "%e %M" -o $i.$1.time.txt ./$i 10000 $1 2500 2500
done
