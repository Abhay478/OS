#!/bin/bash

bash bcas.sh
bash cas.sh
bash tas.sh

pdflatex  -synctex=1 -interaction=nonstopmode -file-line-error -recorder "report.tex"
