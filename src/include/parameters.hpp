////////////////////////////////////////////////////////////////////////////////
/// Copyright 2018-present Xinyan DAI<xinyan.dai@outlook.com>
///
/// permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to
/// deal in the Software without restriction, including without limitation the
/// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
/// sell copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions ofthe Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
/// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
/// IN THE SOFTWARE.

/// @version 0.1
/// @author  Xinyan DAI
/// @contact xinyan.dai@outlook.com
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>


namespace ss{

    struct parameter {

        /// file name of traning data
        std::string train_data;
        /// file name of base data(the data stored in index)
        std::string base_data;
        /// file name of query data
        std::string query_data;
        /// file name of ground truth file(exact top-k nearest neighbor)
        std::string ground_truth;

        /// number of train data
        int train_size;
        /// number of base data(the data stord in index)
        int base_size;
        /// number of query data
        int query_size;

        /// the number of data should be returned
        int topK;

        /// number of thread use
        int num_thread;

        /// for hash method
        /// number of hash value
        int num_bit;

        // number of hash tables
        int forest_size;

        /// dimension of data, equals to transformed_dim+origin_dim
        int dim;
        /// for transform based method, (L2-ALSH Sign-ALSH Simple-LSH)
        int transformed_dim;
        /// for transform based method, dimension of original data
        int origin_dim;

        /// for norm range LSH
        /// number of sub-data-set
        int num_sub_data_set;

        /// for knn-Graph
        /// the degree of knn-Graph
        int graph_K;
        /// for knn-Graph
        /// the knn-graph file
        std::string graph_knn;

        /// training iterations
        int iteration;

        /// for kmeans, PQ, ..
        /// number of centers(in each sub-space)
        int kmeans_centers;
        /// for kmeans, PQ, ..
        /// number of code book
        int num_codebook;

        /// for e2lsh
        /// h(x) = ceil[(av+b)/r]
        float r;

    } typedef parameter;

} // namespace ss
