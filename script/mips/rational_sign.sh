#!/usr/bin/env bash
cd ../../src/
mkdir build/
cd build/
cmake .. -DCMAKE_BUILD_TYPE=Debug
# cmake .. -DCMAKE_BUILD_TYPE=Release
make rational_sign -j16
data_dir="../../data"
data_set="netflix"    # please change it
metric="product"
topk=20               # please change it
code=32               # please change it
log="../../script/plot/data/${data_set}/top${topk}/code${code}/rational_sign_shift_4.sh"
echo "writing logs to : $log"

#gdb --args \
stdbuf -o0  ./rational_sign \
    -t ${data_dir}/${data_set}/${data_set}_base.fvecs \
    -b ${data_dir}/${data_set}/${data_set}_base.fvecs \
    -q ${data_dir}/${data_set}/${data_set}_query.fvecs  \
    -g ${data_dir}/${data_set}/${topk}_${data_set}_${metric}_groundtruth.lshbox \
    --transformed_dim 2 \
    --num_bit 27 \
    --num_sub_data_set ${code} \
    >  $log