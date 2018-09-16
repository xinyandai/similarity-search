#!/usr/bin/env bash
cd ../../src/
mkdir build/ > /dev/null
cd build/
#cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake .. -DCMAKE_BUILD_TYPE=Release  > /dev/null
make rational_alsh -j16  > /dev/null
data_dir="../../data"
metric="product"


topk=$1
data_set=$2
code=$3
num_bit=$4
num_set=$5

mkdir ../../script/plot/data/${data_set}/top${topk}/code${code}/ -p
log="../../script/plot/data/${data_set}/top${topk}/code${code}/rational_alsh_shift_4.sh"
echo "writing logs to : $log"

            #gdb --args \
            timeout 3600 stdbuf -o0  ./rational_alsh \
                -t ${data_dir}/${data_set}/${data_set}_base.fvecs \
                -b ${data_dir}/${data_set}/${data_set}_base.fvecs \
                -q ${data_dir}/${data_set}/${data_set}_query.fvecs  \
                -g ${data_dir}/${data_set}/${topk}_${data_set}_${metric}_groundtruth.lshbox \
                --transformed_dim 3 \
                --num_bit ${num_bit} \
                --num_sub_data_set ${num_set} \
                >  $log
