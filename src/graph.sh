#!/usr/bin/env bash
# rm -rf build/
mkdir build/
cd build/
cmake .. -DCMAKE_BUILD_TYPE=Debug
# cmake .. -DCMAKE_BUILD_TYPE=Release
make graph -j16
data_dir="../../data"
data_set="audio"
metric="euclid"
topk=20
#gdb --args \
./graph \
    -t ${data_dir}/${data_set}/${data_set}_base.fvecs \
    -b ${data_dir}/${data_set}/${data_set}_base.fvecs \
    -q ${data_dir}/${data_set}/${data_set}_query.fvecs  \
    -g ${data_dir}/${data_set}/${topk}_${data_set}_${metric}_groundtruth.lshbox \
    --graph_knn ${data_dir}/${data_set}/${topk}_${data_set}_${metric}_knn.lshbox
