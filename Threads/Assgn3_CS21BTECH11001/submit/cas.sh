#!/bin/bash
echo wc,avg > cas.csv
for n in 10 20 30 40 50; 
do 
    echo $n 10 $1 $2 > inp-params.txt;
    for i in {1..5}; 
    do 
        ./cas
        echo $i
    done
done

python3 cas.py