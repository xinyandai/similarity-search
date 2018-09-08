#!/usr/bin/env bash
cd ../../src/
mkdir build/
cd build/
cmake .. -DCMAKE_BUILD_TYPE=Debug
# cmake .. -DCMAKE_BUILD_TYPE=Release
make cross_polytope_simple_lsh -j16
data_dir="../../data"
metric="product"

centers=4
centers_bit=2

for topk in 20
do
    for data_set in "netflix" "yahoomusic" "imagenet"
    do
        for code in 16 32 64
        do

        log="../../script/plot/data/${data_set}/top${topk}/code${code}/cross_polytope_simple_lsh.sh"
        echo "writing logs to : $log"

        # gdb --args \
        timeout 3600 stdbuf -o0  ./cross_polytope_simple_lsh \
            -t ${data_dir}/${data_set}/${data_set}_base.fvecs \
            -b ${data_dir}/${data_set}/${data_set}_base.fvecs \
            -q ${data_dir}/${data_set}/${data_set}_query.fvecs  \
            -g ${data_dir}/${data_set}/${topk}_${data_set}_${metric}_groundtruth.lshbox \
            --transformed_dim 1 \
            --kmeans_centers ${centers} \
            --num_bit  $((code / centers_bit))\


        done
    done
done