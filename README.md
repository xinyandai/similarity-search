similarity-search
==============
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://github.com/xinyandai/similarity-search/blob/master/LICENSE)
[![Build Status](https://travis-ci.com/xinyandai/similarity-search.svg?token=rQzxktTxAXqqyNh8ZrSa&branch=master)](https://travis-ci.com/xinyandai/similarity-search)
## A general framework for similarity search.
# Reproduce result of Norm-Range LSH


    git clone https://github.com/xinyandai/similarity-search.git
    cd similarity-search
    git checkout mipsex

    cd script/mips/
    sh rational_nr.sh ${topK} ${dataset} ${total_index_bits} ${total_hash_bits} ${num_of_sub_dataset}


You should predefine  ${topK} ${dataset} ${total_index_bits} ${total_hash_bits} ${num_of_sub_dataset},
you can check recall numbers at script/plot/data/${dataset}/top${topK}/code${total_index_bits}/rational_nr_shift_4.sh
after it finished.
for the top-20 of netflix with hash bit length of 12 and 16 sub-dataset

    sh rational_nr.sh 20 netflix 16 12 16 # where total_hash_bits is 16 = 12 + log 16
for the top-20 of netflix with hash bit length of 27 and 32 sub-dataset

    sh rational_nr.sh 20 netflix 32 27 32 # where total_hash_bits is 32 = 27 + log 32

for the top-20 of netflix with hash bit length of 32 and 32 sub-dataset

    sh rational_nr.sh 20 netflix 37 32 32 # where total_hash_bits is 37 = 32 + log 32

For more datasets, you can download the [dataset here](https://xinyandai.github.io/#Datasets). Or you can put your own datasets into folder data/.
After you put the corresponding data into folder data/, then you can use the script /script/tool/cal_groundtruth.sh to calculate the groungtruth.

# style check

    pip install cpplint
    cpplint --recursive --filter=-whitespace,-runtime/indentation_namespace  src/include/*

# Acknowledgement
Our project is developed based on  [GQR](https://github.com//lijinf2/gqr).

# Reference
**[PQ based method for similarity search](https://github.com/xinyandai/product-quantization)**

**[Norm-Ranging LSH for Maximum Inner Product Search](https://arxiv.org/pdf/1809.08782.pdf)**