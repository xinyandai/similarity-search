#!/usr/bin/env bash
cd ../../src/
mkdir build/
cd build/
#cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake .. -DCMAKE_BUILD_TYPE=Release
make sign_alsh -j16
data_dir="../../data"
metric="product"

for topk in 20
do
    for data_set in "netflix" "yahoomusic" "imagenet"
    do
        for code in 16 32 64
        do
            log="../../script/plot/data/${data_set}/top${topk}/code${code}/sign_alsh.sh"
            mkdir ../../script/plot/data/${data_set}/top${topk}/code${code} -p
            echo "writing logs to : $log"

            # gdb --args \
            timeout 3600 stdbuf -o0  ./sign_alsh \
                -t ${data_dir}/${data_set}/${data_set}_base.fvecs \
                -b ${data_dir}/${data_set}/${data_set}_base.fvecs \
                -q ${data_dir}/${data_set}/${data_set}_query.fvecs  \
                -g ${data_dir}/${data_set}/${topk}_${data_set}_${metric}_groundtruth.lshbox \
                --transformed_dim 2 \
                --num_bit ${code} \
                > $log
        done
    done
done
