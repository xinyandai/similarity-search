#!/usr/bin/env bash
cd ../../src/
mkdir build/
cd build/
cmake .. -DCMAKE_BUILD_TYPE=Debug
# cmake .. -DCMAKE_BUILD_TYPE=Release
make sign_alsh -j16
data_dir="../../data"
data_set="netflix"
metric="product"
topk=20
code=32
log="../../script/plot/data/${data_set}/top${topk}/code${code}/sign_alsh.sh"
echo "writing logs to : $log"

# gdb --args \
stdbuf -o0  ./sign_alsh \
    -t ${data_dir}/${data_set}/${data_set}_base.fvecs \
    -b ${data_dir}/${data_set}/${data_set}_base.fvecs \
    -q ${data_dir}/${data_set}/${data_set}_query.fvecs  \
    -g ${data_dir}/${data_set}/${topk}_${data_set}_${metric}_groundtruth.lshbox \
    --transformed_dim 2 \
    --num_bit ${code} \
    > $log
