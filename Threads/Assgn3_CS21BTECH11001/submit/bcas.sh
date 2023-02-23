#!/bin/bash
echo wc,avg > bcas.csv
for n in 10 20 30 40 50; 
do 
    echo $n 10 10 10 > inp-params.txt;
    for i in {1..5}; 
    do 
        ./bcas
        echo $i
    done
done

python3 bcas.py