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

#include <vector>
#include <functional>

#include "heap.hpp"

namespace ss {

    using std::vector;

    /// TODO(Xinyan); to be improved
    template<typename DataType >
    vector<vector<MaxHeapElement<int> > > ExactKNN(
            const DataType * queries,
            const int num_queries,
            const DataType * items,
            const int num_items,
            int dim,
            int K,
            std::function<float(const DataType *, const DataType *, const int )> dist) {

        using HeapType = Heap<int, DataType, MaxHeapElement<int, DataType>> ;

        vector<HeapType> heaps(num_queries, HeapType(K));
        vector<vector<MaxHeapElement<int>>> knns(num_queries);

#pragma omp for dynamic
        for (size_t query_id = 0; query_id < num_queries; ++query_id) {
            for (size_t item_id = 0; item_id < num_items; ++item_id) {
                heaps[query_id].Insert(dist(&queries[query_id * dim], &items[item_id * dim], dim), item_id);
            }
            knns[query_id] = heaps[query_id].GetTopK();
        }

        return knns;
    }

} // namespace ss
