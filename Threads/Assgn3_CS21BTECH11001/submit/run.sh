#!/bin/bash

echo Output: > output.txt
IFS=' '
read -r line < inp-params.txt
read -ra params <<< $line

echo BCAS
bash bcas.sh "${params[2]}" "${params[3]}"

echo CAS
bash cas.sh "${params[2]}" "${params[3]}"

echo TAS
bash tas.sh "${params[2]}" "${params[3]}"

python3 wc.py
python3 avg.py
rm bcas cas tas *.csv
