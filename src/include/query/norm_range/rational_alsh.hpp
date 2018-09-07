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
    class RationalALSHRanking :
            public Query <DataType> {
        using MapType   = unordered_map<KeyType, vector<int>, ss::SSHasher<KeyType> >;
        using IndexType = NormRangeIndex<DataType, KeyType, HashingIndexType, Transformer >;
    private:
        const MapType &                          _index_map;
        BucketSorter<DataType, KeyType > *       _sorter;
    public:
        ~RationalALSHRanking() {}

        explicit RationalALSHRanking(
                IndexType *                        index,
                const DataType *                   query,
                const Metric<DataType > &          metric,
                const Matrix<DataType > &          data,
                const parameter &                  para)
                : Query<DataType >(index, query, metric, data, para),
                  _index_map(index->getIndexMap()){

            { /// construction here,

                /// calculate query's hash value by hash function
                KeyType query_hash = index->HashQuery(query, -1);

                /// distance function from query to bucket(designated by index_key)
                auto distor = [this, &query_hash, &para, index](const KeyType & index_key) {

                    std::pair<int, DataType > hash_dist_and_norm
                            = index->HashDistAndPercentile(index_key, query_hash);
                    int num_same_bit = para.num_bit - hash_dist_and_norm.first;
                    DataType u = hash_dist_and_norm.second;
                    return this->distance(num_same_bit, u);
                };
                _sorter = new BucketSorter<DataType, KeyType>(_index_map, distor);

            } /// end construction
        }


        double NormalCFD(double value) {
            return 0.5 * erfc(-value * M_SQRT1_2);
        }

        double Fr(double d) {
            double r_d = this->_para.r / d;
            return 1.0 - 2.0*NormalCFD(-r_d)
                   - 2.0 * (1 - std::pow(M_E, -r_d*r_d/2.0)) / std::sqrt(2.0 * M_PI ) / r_d;
        }

        double DistanceByProbability(double p) {
            double mid;
            double a = 0.0;
            int m = this->_para.transformed_dim;
            double u = 0.83;
            double b = std::sqrt(1+m/4.0+std::pow(u, 1<<(m+1)) + 2 * u); //upper bound of distance

            int i=0;
            while(i < 500) {
                mid=(a+b)/2;
                if(std::abs(p - Fr(mid)) < 0.00001)
                    return mid;
                if((Fr(mid)-p)*(Fr(a)-p)>0)
                    a=mid;
                else
                    b=mid;
                i++;
            }
            return mid;
        }

        DataType distance(int num_same_bit, DataType u)  {
            DataType probability = num_same_bit / static_cast<DataType>(this->_para.num_bit);
            DataType shift       = 0.40f;
            probability          = shift + (1 - shift) * probability;
            double d = DistanceByProbability(probability);
            DataType dist = - u * (1 + this->_para.transformed_dim/4 - d * d);
            return dist;
        }

        const vector<int> &  NextBucket() override {
            typename MapType::const_iterator bucket;
            while ( (bucket = _index_map.find(_sorter->NextBucket()) ) ==_index_map.end()) {}
            return bucket->second;
        }

        bool NextBucketExisted() const override {
            return _sorter->NextBucketExisted();
        }
    };
}  //end namespace ss