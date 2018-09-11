#!/usr/bin/env bash
cd ../../src/
mkdir build/
cd build/
#cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake .. -DCMAKE_BUILD_TYPE=Release
make rational_nr -j16
data_dir="../../data"
metric="product"

codes=(16 32 64)
bits=(12 27 58)
sets=(16 32 64)

for topk in 20
do
    for data_set in "netflix" "yahoomusic" "imagenet"
    do
        for i in 0 1 2
        do
            code=${codes[$i]}
            num_bit=${bits[$i]}
            num_set=${sets[$i]}

            log="../../script/plot/data/${data_set}/top${topk}/code${code}/rational_nr_shift_4.sh"
            mkdir ../../script/plot/data/${data_set}/top${topk}/code${code} -p
            echo "writing logs to : $log"

            #gdb --args \
            timeout 3600 stdbuf -o0  ./rational_nr \
                -t ${data_dir}/${data_set}/${data_set}_base.fvecs \
                -b ${data_dir}/${data_set}/${data_set}_base.fvecs \
                -q ${data_dir}/${data_set}/${data_set}_query.fvecs  \
                -g ${data_dir}/${data_set}/${topk}_${data_set}_${metric}_groundtruth.lshbox \
                --transformed_dim 1 \
                --num_bit ${num_bit} \
                --num_sub_data_set ${num_set} \
                >  $log
        done
    done
done