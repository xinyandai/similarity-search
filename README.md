similarity-search
==============
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT) 
[![Build Status](https://travis-ci.com/xinyandai/similarity-search.svg?token=rQzxktTxAXqqyNh8ZrSa&branch=master)](https://travis-ci.com/xinyandai/similarity-search)
## A general framework for similarity search.
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
        - PCAH,ITQ,...(not develop yet)
      - int index
        - e2lsh
        - KMeans,PQ,OPQ...(not develop yet)
  - graph based index(not develop yet)
  - others
    - norm-range(for max inner product search)
#  query method
  - int rank
  - hamming rank
  - others
    - norm-range(for norm-range index only)
