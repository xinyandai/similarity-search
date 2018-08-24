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
#include <unordered_map>

#include "sorter/radix_sorter.hpp"

#include "../parameters.hpp"
#include "../query.hpp"
#include "../index/map_index.hpp"
#include "../utils/calculator.hpp"

namespace ss {

    template <class DataType>
    class IntRanker : public Query<DataType > {

        using KeyType  = vector<int >;
        using HashType = std::unordered_map<KeyType, vector<int>, ss::SSHasher<KeyType > >;

    protected:
        const HashType &                          _index_map;
        ss::RadixSorter<DataType, KeyType> *      _sorter;

    public:
        ~IntRanker() { if(_sorter) delete _sorter; }

        explicit IntRanker(
                MapIndex<DataType, vector<int> > * index,
                const DataType *                   query,
                const Metric<DataType > &          metric,
                const Matrix<DataType > &          data,
                const parameter &                  para)
                :
                Query<DataType >(index, query, metric, data, para), _index_map(index->getIndexMap()) {

            { /// construction here

                KeyType query_hash = index->HashQuery(query, -1);
                /// distance function from query to bucket(designated by index_key)
                auto distor = [query_hash] (const KeyType & index_key) {
                    return ss::CountDiff(query_hash.data(), index_key.data(), query_hash.size());
                };
                /// sorter helper
                _sorter = new RadixSorter<DataType, KeyType>(index->getIndexMap(), distor, para.num_bit);

            } /// end construction
        }

        const vector<int > & NextBucket() override {
            HashType::const_iterator bucket;
            while ( (bucket = _index_map.find(_sorter->NextBucket()) ) == _index_map.end()) {}
            return bucket->second;
        }

        bool NextBucketExisted() const override   {
            return _sorter->NextBucketExisted();
        }
    
    };

} // namespace ss
