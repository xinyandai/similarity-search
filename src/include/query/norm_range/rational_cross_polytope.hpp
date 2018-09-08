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

#include <cmath>
#include <utility>
#include <vector>
#include <index/pq.hpp>
#include <boost/math/special_functions/beta.hpp>

#include "query/sorter/bucket_sorter.hpp"

#include "parameters.hpp"
#include "query.hpp"
#include "index/transformer/norm_range.hpp"

namespace ss {


    template<
            class DataType,
            class KeyType,
            class HashingIndexType,
            class Transformer
    >
    class RationalCrossPolytopeRanking :
            public Query <DataType > {

        using MapType   = unordered_map<KeyType, vector<int>, ss::SSHasher<KeyType> >;
        using IndexType = NormRangeIndex<DataType, KeyType, HashingIndexType, Transformer >;
    private:
        const MapType &                          _index_map;
        BucketSorter<DataType, KeyType > *      _sorter;

    public:
        ~RationalCrossPolytopeRanking() {}

        explicit RationalCrossPolytopeRanking(
                IndexType *                        index,
                const DataType *                   query,
                const Metric<DataType > &          metric,
                const Matrix<DataType > &          data,
                const parameter &                  para)
                : Query<DataType >(index, query, metric, data, para),
                  _index_map(index->getIndexMap()){

            { /// construction here


                CrossPolytopeIndex<DataType> * cross_polytope = index->GetIndex();
                vector<int >  query_hash = cross_polytope->HashQuery(query, -1);
                vector<vector<DataType > > xs = cross_polytope->Rotate(query);
                vector<int >      max_indexes = cross_polytope->ArgMaxAbs(xs);
                /// distance function from query to bucket(in each sub data set)
                auto inner_distor = [&xs, &max_indexes, &query_hash, &para, index] (const KeyType & index_key) {
                    DataType cost_metric = 0.0f;
                    for (int k = 0; k < xs.size(); ++k) {

                        DataType max_absolute = std::abs(xs[k][max_indexes[k]]);
                        DataType query_value = xs[k][ std::abs(index_key[k]) - 1 ]; ///index begin with 0
                        /// the cost to make absolute value of query[index_key[k]] bigger than actual 'max_absolute'
                        cost_metric += max_absolute - query_value * (index_key[k]>0?1:-1);
                    }

                    DataType distance_square = 2.0 - 2.0 / std::sqrt(cost_metric+1.0);
                    DataType u = index->Percentile(index_key);
                    return  - u * (8.0 - distance_square);
                };


                /// sorter helper
                _sorter = new BucketSorter<DataType, KeyType>(_index_map, inner_distor);
            } /// end construction
        }

        const vector<int> &  NextBucket() override {
            typename MapType::const_iterator bucket;
            while ( (bucket = _index_map.find(_sorter->NextBucket()) ) ==_index_map.end()) {}
            return bucket->second;
        }
    };
}  // end namespace ss