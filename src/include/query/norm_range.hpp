//////////////////////////////////////////////////////////////////////////////
/// Copyright 2018-present Xinyan DAI<xinyan.dai@outlook.com>
///
/// permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
/// documentation files (the "Software"), to deal in the Software without restriction, including without limitation
/// the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
/// and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in all copies or substantial portions of
/// the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
/// THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
/// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.

/// @version 0.1
/// @author  Xinyan DAI
/// @contact xinyan.dai@outlook.com
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cmath>

#include "sorter/bucket_sorter.hpp"

#include "../parameters.hpp"
#include "../query.hpp"
#include "../index/norm_range.hpp"

namespace ss {

    using namespace lshbox;

    template <class DataType>
    class NormRangeQuery : public Query<DataType > {

        using AccessorType = typename lshbox::Matrix<DataType>::Accessor;
        using KeyType      = uint64_t;

    protected:
        unordered_map<KeyType, vector<int>, ss::SSHasher<KeyType> > &	_index_map;
        BucketSorter<DataType, KeyType > *								_bucket_sorter;

    public:
        ~NormRangeQuery() {delete _bucket_sorter; }

        explicit NormRangeQuery(
				NormRangeIndex<DataType > * 	index,
                DataType * 			            query,
                lshbox::Metric<DataType > & 	metric,
                const AccessorType & 		    accessor,
                parameter & 			        para)
                :
                Query<DataType >(index, query, metric, accessor, para),
                _index_map(index->getIndexMap()) {

            KeyType query_hash = index->HashQuery(query, -1);
            const DataType PI = std::acos(-1);

            auto distor = [&query_hash, &para, index, PI](const KeyType & key) {
                std::pair<int, DataType > hash_dist_and_norm = index->HashDistAndPercentile(key, query_hash);
                int num_same_bit = para.num_bit - hash_dist_and_norm.first;
                DataType u = hash_dist_and_norm.second;
                DataType dist = - (num_same_bit * u * u  ) ;
                // DataType probability = num_same_bit / (DataType)para.num_bit;
                // DataType scaled_probability = std::min(1.0f, probability + std::sqrt(1 * probability * (1-probability) / para.num_bit));
                // DataType alpha = 0.3f;
                // DataType scaled_probability = std::min(1.0f, probability * (1-alpha) + alpha);
                // DataType dist = - u * std::cos( PI * (1.0f - scaled_probability) );
                return dist;
            };
            _bucket_sorter = new BucketSorter<DataType, KeyType>(_index_map, distor);
        }


        const vector<int> &	NextBucket() override { return _index_map[_bucket_sorter->NextBucket()]; }

        bool NextBucketExisted() const override   { return _bucket_sorter->NextBucketExisted(); }

    };
}
