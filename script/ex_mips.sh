#!/usr/bin/env bash
cd mips

codes=(16 32 64)
bits=(12 27 58)
sets=(16 32 64)

for topk in 1 10 50
do
    for data_set in "netflix" "yahoomusic" "imagenet"
    do
        for i in 0 1 2
        do
            for file in `ls `
            do
                sh $file $topk $data_set ${codes[$i]} ${bits[$i]} ${sets[$i]}

            done
        done
    done
done