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

#include "query/norm_range/sub_data_sorter.hpp"
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
                    DataType distance = 0.0f;
                    for (int k = 0; k < xs.size(); ++k) {

                        DataType max_absolute = std::abs(xs[k][max_indexes[k]]);
                        DataType query_value = xs[k][ std::abs(index_key[k]) - 1 ]; ///index begin with 0
                        /// the cost make absolute value of query[index_key[k]] bigger than actual 'max_absolute'
                        distance += max_absolute - query_value * (index_key[k]>0?1:-1);
                    }

                    return  distance;
                };

                /// distance function between different sub-data-set
                auto inter_distor = [this, &query_hash, &para, index](const KeyType & index_key) {

                    std::pair<int, DataType > hash_dist_and_norm
                            = index->HashDistAndPercentile(index_key, query_hash);
                    int num_same_bit = para.num_bit - hash_dist_and_norm.first;
                    DataType u = hash_dist_and_norm.second;
                    return this->distance(num_same_bit, u);
                };
                /// sorter helper
                _sorter = new BucketSorter<DataType, KeyType>(_index_map, inner_distor);
            } /// end construction
        }


        DataType distance(int num_same_bit, DataType u)  {
            DataType probability = num_same_bit / static_cast<DataType>(this->_para.num_bit);
            DataType shift       = 0.0f;
            probability          = shift + (1 - shift) * probability;
            DataType euclid      = DistanceByProbability(probability);
            DataType ip_proxy    = u * (4 - euclid * euclid );
            return - ip_proxy;
        }


        double Fr(double t) {

            double d = 2.0;
            return 2.0 * d * (d-1) * boost::math::beta((8.0 - t*t) / ( 4.0 - t*t), d-1);
        }

        double DistanceByProbability(double p) {
            double mid;
            double a = 0.0;
            double b = 2.0; //upper bound of distance

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

        const vector<int> &  NextBucket() override {
            typename MapType::const_iterator bucket;
            while ( (bucket = _index_map.find(_sorter->NextBucket()) ) ==_index_map.end()) {}
            return bucket->second;
        }

    };
}  // end namespace ss