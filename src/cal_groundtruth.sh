#!/usr/bin/env bash
# rm -rf build/
mkdir build/
cd build/
cmake .. -DCMAKE_BUILD_TYPE=Debug
# cmake .. -DCMAKE_BUILD_TYPE=Release
make ground_truth -j16

topk=20
numThreads=16
# metric="product"
 metric="angular"
#metric="euclid"


for dataset in "audio"
do

    # input data files
    base_file="../../data/${dataset}/${dataset}_base.fvecs"
    query_file="../../data/${dataset}/${dataset}_query.fvecs"

    # output groundtruth files, support both ivecs and lshbox formats
    ivecs_bench_file="../../data/${dataset}/${topk}_${dataset}_${metric}_groundtruth.ivecs"
    lshbox_bench_file="../../data/${dataset}/${topk}_${dataset}_${metric}_groundtruth.lshbox"

    ./ground_truth $base_file $query_file $topk $lshbox_bench_file $ivecs_bench_file $metric $numThreads
done
