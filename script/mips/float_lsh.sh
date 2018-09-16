#!/usr/bin/env bash
cd ../../src/
mkdir build/ > /dev/null
cd build/
#cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake .. -DCMAKE_BUILD_TYPE=Release  > /dev/null
make float_lsh -j16  > /dev/null
data_dir="../../data"
data_set="netflix"
metric="product"
topk=20
code=16

mkdir ../../script/plot/data/${data_set}/top${topk}/code${code}/ -p
log="../../script/plot/data/${data_set}/top${topk}/code${code}/float_lsh.sh"
echo "writing logs to : $log"

# gdb --args \
stdbuf -o0  ./float_lsh \
    -t ${data_dir}/${data_set}/${data_set}_base.fvecs \
    -b ${data_dir}/${data_set}/${data_set}_base.fvecs \
    -q ${data_dir}/${data_set}/${data_set}_query.fvecs  \
    -g ${data_dir}/${data_set}/${topk}_${data_set}_${metric}_groundtruth.lshbox \
    --transformed_dim 1 \
    --num_bit ${code}
#    \
#    > $log
