#pragma once

#include <math.h>
#include <assert.h>

#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <functional>
#include <cmath>
#include <thread>
#include <utility>
#include <fstream>

#include "heap.hpp"

using namespace std;

namespace ss {

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


}
