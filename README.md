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
        - OPQ(to be developed)
  - graph based index(to be developed)
  - transform based method
    - simple-LSH(for maximum inner product search)
    - norm-range(for maximum inner product search)
    - l2-ALSH(to be developed)

#  query method
  - int ranking(for int index)
  - hamming ranking(for bit index)
  - cluster ranker(for cluster based method, k-means only)
  - inverted multi index(for pq only)
  - norm-range(for norm-range index only)

# style check

    pip install cpplint
    cpplint --recursive --filter=-whitespace,-build/namespaces,-runtime/references,-readability/namespace,-build/include_what_you_use,-runtime/indentation_namespace,-build/include_subdir  src/include/*

