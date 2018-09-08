#!/usr/bin/env bash
cd ../../src/
mkdir build/
cd build/
#cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake .. -DCMAKE_BUILD_TYPE=Release
make cross_polytope -j16
data_dir="../../data"
data_set="audio"
metric="angular"
topk=20

centers=4
centers_bit=2
num_bit=64

#gdb --args \
./cross_polytope \
    -t ${data_dir}/${data_set}/${data_set}_base.fvecs \
    -b ${data_dir}/${data_set}/${data_set}_base.fvecs \
    -q ${data_dir}/${data_set}/${data_set}_query.fvecs  \
    -g ${data_dir}/${data_set}/${topk}_${data_set}_${metric}_groundtruth.lshbox \
    --kmeans_centers ${centers} \
    --num_bit  $((num_bit / centers_bit))\
