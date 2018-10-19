similarity-search
==============
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://github.com/xinyandai/similarity-search/blob/master/LICENSE)
[![Build Status](https://travis-ci.com/xinyandai/similarity-search.svg?token=rQzxktTxAXqqyNh8ZrSa&branch=master)](https://travis-ci.com/xinyandai/similarity-search)
## A general framework for similarity search.
# Run
 script

    git clone https://github.com/xinyandai/similarity-search.git
    cd similarity-search
    sh srp.sh

 command

    git clone https://github.com/xinyandai/similarity-search.git
    cd similarity-search/src
    mkdir build
    cd build
    cmake ..
    make srp
    ./srp -h
    ./srp -t ${train_data_file} -b ${base_data_file} -q ${query_data_file}  -g ${ground_truth_file}

You should predefine  ${train_data_file} ${base_data_file} ${query_data_file} ${ground_truth_file}.
# process
  - 1. choose the index structure and query method
  - 2. load data, include training data, base data and query data, ground truth is needed for recall.
  - 3. training the index using training data
  - 4. store the base data in index
  - 5. for each query, probe high priority items/buckets using the choosed query method
  - 6. for each query, re-rank the probed items
# index type
  - map based index
      - bit index
        - sign random projection
        - ITQ
        - PCAH(to be developed)
      - int index
        - e2lsh
      - cluster index
        - k-means
        - PQ
  - graph based index(to be developed)
  - transform based method(for maximum inner product search)
    - Simple-LSH
    - Norm-Range LSH
    - L2-ALSH
    -

#  query method
  - int ranking(for int index)
  - hamming ranking(for bit index)
  - cluster ranker(for cluster based method)
  - inverted multi index(for pq only)
  - norm-range(for norm-range index only)

# style check

    pip install cpplint
    cpplint --recursive --filter=-whitespace,-runtime/indentation_namespace  src/include/*

# Acknowledgement
Our project is developed based on  [GQR](https://github.com//lijinf2/gqr).

# Reference
**[PQ based method for similarity search](https://github.com/xinyandai/product-quantization)**

**[Norm-Ranging LSH for Maximum Inner Product Search](https://arxiv.org/pdf/1809.08782.pdf)**