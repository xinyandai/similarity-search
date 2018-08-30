#!/usr/bin/env bash
cd ../../src/
mkdir build/
cd build/
cmake .. -DCMAKE_BUILD_TYPE=Debug
# cmake .. -DCMAKE_BUILD_TYPE=Release
make norm_range -j16
data_dir="../../data"
data_set="netflix"
metric="product"
topk=20
code=32
log="../../script/plot/data/${data_set}/top${topk}/code${code}/norm_range.sh"
echo "writing logs to : $log"

#gdb --args \
stdbuf -o0   ./norm_range \
    -t ${data_dir}/${data_set}/${data_set}_base.fvecs \
    -b ${data_dir}/${data_set}/${data_set}_base.fvecs \
    -q ${data_dir}/${data_set}/${data_set}_query.fvecs  \
    -g ${data_dir}/${data_set}/${topk}_${data_set}_${metric}_groundtruth.lshbox \
    --transformed_dim 1 \
    --num_bit 27 \
    --num_sub_data_set ${code} \
    > $log