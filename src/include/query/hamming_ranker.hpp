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

#include <bitset>
#include <functional>
#include <vector>

#include "sorter/radix_sorter.hpp"

#include "../parameters.hpp"
#include "../query.hpp"
#include "../index/map_index.hpp"

namespace ss {


    template <class DataType>
    class HammingRanker : public Query<DataType > {

        using KeyType  = uint64_t;
        using HashType = std::unordered_map<KeyType, vector<int >, ss::SSHasher<KeyType> >;

    protected:
        const HashType &                    _index_map;
        RadixSorter<DataType, KeyType> *    _sorter; /// sorting helper

    public:
        ~HammingRanker() { if(_sorter) delete _sorter; }

        explicit HammingRanker(
                MapIndex<DataType, KeyType > *     index,
                const DataType *                   query,
                const Metric<DataType > &          metric,
                const Matrix<DataType > &          data,
                const parameter &                  para)
                :
                Query<DataType >(index, query, metric, data, para),
                _index_map(index->getIndexMap()) {

            { /// construction here

                KeyType query_hash = index->HashQuery(query, -1);
                /// distance function from query to bucket(designated by index_key)
                auto distor = [query_hash](const KeyType & index_key) {
                    return ss::CountBitOne(query_hash ^ index_key) ;
                };
                /// sorting helper
                _sorter = new RadixSorter<DataType, KeyType>(index->getIndexMap(), distor, para.num_bit);

            } /// end construction
        }

        const vector<int> & NextBucket() override {
            HashType::const_iterator bucket;
            while ( (bucket = _index_map.find(_sorter->NextBucket()) ) ==_index_map.end()) {}
            return bucket->second;
        }

        bool NextBucketExisted() const override {
            return _sorter->NextBucketExisted();
        }
    
    };

} // namespace ss
