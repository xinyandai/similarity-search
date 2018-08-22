#!/usr/bin/env bash
# rm -rf build/
mkdir build/
cd build/
cmake .. -DCMAKE_BUILD_TYPE=Debug
# cmake .. -DCMAKE_BUILD_TYPE=Release
make norm_range
data_dir="../../data"
data_set="netflix"
metric="product"
topk=20

#gdb --args \
./norm_range \
    -t ${data_dir}/${data_set}/${data_set}_base.fvecs \
    -b ${data_dir}/${data_set}/${data_set}_base.fvecs \
    -q ${data_dir}/${data_set}/${data_set}_query.fvecs  \
    -g ${data_dir}/${data_set}/${topk}_${data_set}_${metric}_groundtruth.lshbox \
    --transformed_dim 1 \
    --num_bit 27 \
    --num_sub_data_set 32
