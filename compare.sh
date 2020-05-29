#!/bin/bash

cktsA=("cla32" "ksa32")
ckts=("alu4" "c880" "c1908" "c3540" "cla32" "ksa32" "mtp8" "rca32" "wal8")
for P1 in ${ckts[*]}
do
    orig=data/aig/${P1}/${P1}_orig.aig
    for P2 in 0.005 0.05 #0.0002 0.002
    do
        for F3 in data/aig/${P1}/${P2}/*
        do  
            P3=`basename $F3`
            echo ${P1} ${P3} 
            ./main 1000000 ${orig} data/aig/${P1}/${P2}/${P3}
        done
    done
done
for P1 in ${cktsA[*]}
do
    orig=data/aig/${P1}/${P1}_orig.aig
    for P2 in 0.0002 0.002
    do
        for F3 in data/aig/${P1}/${P2}/*
        do  
            P3=`basename $F3`
            echo ${P1} ${P2} ${P3} 
            ./main 1000000 ${orig} data/aig/${P1}/${P2}/${P3}
        done
    done
done