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
    cmake ..
    make all -j

### Run Simple-LSH

    ./simple_lsh \
        -t ../../data/netflix/netflix_base.fvecs  \
        -b ../../data/netflix/netflix_base.fvecs  \
        -q ../../data/netflix/netflix_query.fvecs \
        -g ../../data/netflix/20_netflix_product_groundtruth.lshbox \
        --transformed_dim 1 \
        --num_bit 32

### Run [Norm-Ranging LSH](https://arxiv.org/pdf/1809.08782.pdf)(Rational-Simple-LSH)

    ./rational_nr \
        -t ../../data/netflix/netflix_base.fvecs  \
        -b ../../data/netflix/netflix_base.fvecs  \
        -q ../../data/netflix/netflix_query.fvecs \
        -g ../../data/netflix/20_netflix_product_groundtruth.lshbox \
        --transformed_dim 1 \
        --num_bit 27\
        --num_sub_data_set 32


### Run L2-ALSH

    ./alsh \
        -t ../../data/netflix/netflix_base.fvecs  \
        -b ../../data/netflix/netflix_base.fvecs  \
        -q ../../data/netflix/netflix_query.fvecs \
        -g ../../data/netflix/20_netflix_product_groundtruth.lshbox \
        --transformed_dim 3 \
        --num_bit 32
### Run Rational-L2-ALSH

    ./rational_alsh \
        -t ../../data/netflix/netflix_base.fvecs  \
        -b ../../data/netflix/netflix_base.fvecs  \
        -q ../../data/netflix/netflix_query.fvecs \
        -g ../../data/netflix/20_netflix_product_groundtruth.lshbox \
        --transformed_dim 3 \
        --num_bit 27 \
        --num_sub_data_set 32
### Run Sign-ALSH

    ./sign_alsh \
        -t ../../data/netflix/netflix_base.fvecs  \
        -b ../../data/netflix/netflix_base.fvecs  \
        -q ../../data/netflix/netflix_query.fvecs \
        -g ../../data/netflix/20_netflix_product_groundtruth.lshbox \
        --transformed_dim 2 \
        --num_bit 32
        
### Run Rational-Sign-ALSH

    ./rational_sign \
        -t ../../data/netflix/netflix_base.fvecs  \
        -b ../../data/netflix/netflix_base.fvecs  \
        -q ../../data/netflix/netflix_query.fvecs \
        -g ../../data/netflix/20_netflix_product_groundtruth.lshbox \
        --transformed_dim 2 \
        --num_bit 27 \
        --num_sub_data_set 32
                
### Run Cross-Polytope-Simple-LSH

    ./cross_polytope_simple_lsh \
        -t ../../data/netflix/netflix_base.fvecs  \
        -b ../../data/netflix/netflix_base.fvecs  \
        -q ../../data/netflix/netflix_query.fvecs \
        -g ../../data/netflix/20_netflix_product_groundtruth.lshbox \
        --transformed_dim 1 \
        --kmeans_centers 256 \
        --num_bit  4
        
### Run Rational Cross-Polytope-Simple-LSH

    ./rational_cross_polytope \
        -t ../../data/netflix/netflix_base.fvecs  \
        -b ../../data/netflix/netflix_base.fvecs  \
        -q ../../data/netflix/netflix_query.fvecs \
        -g ../../data/netflix/20_netflix_product_groundtruth.lshbox \
        --transformed_dim 1 \
        --num_sub_data_set 16 \
        --kmeans_centers 16 \
        --num_bit  4
