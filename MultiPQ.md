similarity-search
==============
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://github.com/xinyandai/similarity-search/blob/master/LICENSE)
[![Build Status](https://travis-ci.com/xinyandai/similarity-search.svg?token=rQzxktTxAXqqyNh8ZrSa&branch=master)](https://travis-ci.com/xinyandai/similarity-search)
## A general framework for similarity search.
### make all  
    git clone https://github.com/xinyandai/similarity-search.git
    cd similarity-search/src
    mkdir build
    cd build
    cmake  -DCMAKE_BUILD_TYPE=Release  .. -DMY_BOOST="/research/jcheng2/xinyan/env/boost_install/lib/" -DMY_EIGEN="/research/jcheng2/xinyan/env/eigen3_install/include/eigen3"
    # make all -j

### Run Joint Inverted Index

    make joint_inverted -j
    dataset="imagenet" ./joint_inverted  \
        -t ../../data/${dataset}/${dataset}_base.fvecs \
        -b ../../data/${dataset}/${dataset}_base.fvecs \
        -q ../../data/${dataset}/${dataset}_query.fvecs \
        -g ../../data/${dataset}/20_${dataset}_euclid_groundtruth.lshbox \
        --num_bit 8 --forest_size 6 --kmeans_centers 256

### Run Multi PQ
    make multi_pq_ranker -j
    dataset="audio" ./multi_pq_ranker  \
        -t ../../data/${dataset}/${dataset}_base.fvecs \
        -b ../../data/${dataset}/${dataset}_base.fvecs \
        -q ../../data/${dataset}/${dataset}_query.fvecs \
        -g ../../data/${dataset}/20_${dataset}_euclid_groundtruth.lshbox \
        --num_bit 8 --forest_size 4 --kmeans_centers 256
        
