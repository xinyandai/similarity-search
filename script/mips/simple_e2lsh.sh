#!/usr/bin/env bash
cd ../../src/
mkdir build/
cd build/
#cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake .. -DCMAKE_BUILD_TYPE=Release
make simple_e2lsh -j16
data_dir="../../data"
metric="product"



for topk in 20
do
    for data_set in "netflix"
    do
        for code in 32
        do
            log="../../script/plot/data/${data_set}/top${topk}/code${code}/simple_e2lsh.sh"
            mkdir ../../script/plot/data/${data_set}/top${topk}/code${code} -p
            echo "writing logs to : $log"

            # gdb --args \
            timeout 3600 stdbuf -o0  ./simple_e2lsh \
                -t ${data_dir}/${data_set}/${data_set}_base.fvecs \
                -b ${data_dir}/${data_set}/${data_set}_base.fvecs \
                -q ${data_dir}/${data_set}/${data_set}_query.fvecs  \
                -g ${data_dir}/${data_set}/${topk}_${data_set}_${metric}_groundtruth.lshbox \
                --transformed_dim 1 \
                --num_bit ${code} \
                > $log
        done
    done
done