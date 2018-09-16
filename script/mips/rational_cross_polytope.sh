#!/usr/bin/env bash
cd ../../src/
mkdir build/ > /dev/null
cd build/
#cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake .. -DCMAKE_BUILD_TYPE=Release  > /dev/null
make rational_cross_polytope -j16  > /dev/null
data_dir="../../data"
metric="product"

topk=$1
data_set=$2
code=$3
num_bit=$4
num_set=$5

centers=4
centers_bit=2

mkdir ../../script/plot/data/${data_set}/top${topk}/code${code}/ -p
log="../../script/plot/data/${data_set}/top${topk}/code${code}/rational_cross_polytope.sh"
echo "writing logs to : $log"

            #gdb --args \
            timeout 3600 stdbuf -o0 ./rational_cross_polytope \
                -t ${data_dir}/${data_set}/${data_set}_base.fvecs \
                -b ${data_dir}/${data_set}/${data_set}_base.fvecs \
                -q ${data_dir}/${data_set}/${data_set}_query.fvecs  \
                -g ${data_dir}/${data_set}/${topk}_${data_set}_${metric}_groundtruth.lshbox \
                --transformed_dim 1 \
                --num_sub_data_set ${num_set} \
                --kmeans_centers ${centers} \
                --num_bit  $((num_bit / centers_bit))\
                > $log
