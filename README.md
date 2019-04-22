similarity-search
==============
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://github.com/xinyandai/similarity-search/blob/master/LICENSE)
[![Build Status](https://travis-ci.com/xinyandai/similarity-search.svg?token=rQzxktTxAXqqyNh8ZrSa&branch=master)](https://travis-ci.com/xinyandai/similarity-search)
## A general framework for similarity search.
### To reproduce result of our NIPS paper, please refer [MIPS](https://github.com/xinyandai/similarity-search/blob/master/MIPS.md).
### make all  
    git clone https://github.com/xinyandai/similarity-search.git
    cd similarity-search/src
    mkdir build
    cd build
    cmake  -DCMAKE_BUILD_TYPE=Release  .. -DMY_BOOST="/research/jcheng2/xinyan/env/boost_install/lib/" -DMY_EIGEN="/usr/include/eigen3"
    # make all -j

### Run PQ-IMI

    ./pq \
        -t ../../data/audio/audio_base.fvecs  \
        -b ../../data/audio/audio_base.fvecs  \
        -q ../../data/audio/audio_query.fvecs \
        -g ../../data/audio/20_audio_euclid_groundtruth.lshbox \
        --iteration 20 \
        --kmeans_centers 20 \
        --num_codebook 3

### Run Sign Random Projection

    ./srp \
        -t ../../data/audio/audio_base.fvecs  \
        -b ../../data/audio/audio_base.fvecs  \
        -q ../../data/audio/audio_query.fvecs \
        -g ../../data/audio/20_angular_euclid_groundtruth.lshbox \
        --num_bit 32

### Run Cross Polytope LSH

    ./cross_polytope \
        -t ../../data/audio/audio_base.fvecs  \
        -b ../../data/audio/audio_base.fvecs  \
        -q ../../data/audio/audio_query.fvecs \
        -g ../../data/audio/20_angular_euclid_groundtruth.lshbox \
        --kmeans_centers 32\
        --num_bit  1
    # kmeans_centers represent d' and num_bit represents the number of hash tables
### Run Iterative Quantization

    ./itq \
        -t ../../data/audio/audio_base.fvecs  \
        -b ../../data/audio/audio_base.fvecs  \
        -q ../../data/audio/audio_query.fvecs \
        -g ../../data/audio/20_audio_euclid_groundtruth.lshbox \
        --num_bit 32
### Run E2LSH

    ./e2lsh \
        -t ../../data/audio/audio_base.fvecs  \
        -b ../../data/audio/audio_base.fvecs  \
        -q ../../data/audio/audio_query.fvecs \
        -g ../../data/audio/20_audio_euclid_groundtruth.lshbox \
        --num_bit 32

## Reference
**[PQ based method for gradient quantization](https://github.com/xinyandai/gradient-quantization)**

**[PQ based method for similarity search](https://github.com/xinyandai/product-quantization)**

**[Norm-Ranging LSH for Maximum Inner Product Search](https://arxiv.org/pdf/1809.08782.pdf)**

**[Norm-Range Partition: A Universal Catalyst for LSH based Maximum Inner Product Search (MIPS)](https://arxiv.org/pdf/1810.09104.pdf)**