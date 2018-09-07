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

#include "sorter/bucket_sorter.hpp"

#include "../parameters.hpp"
#include "../query.hpp"
#include "../index/cross_polytype.hpp"
#include "../index/transformer/sign_transformer.hpp"
#include "../index/transformer/simple_transformer.hpp"
#include "../index/transformer/transformer_lsh.hpp"
#include "../utils/calculator.hpp"

namespace ss {

    template <class DataType>
    class CrossPolytopeRanker : public Query<DataType > {

        using KeyType  = vector<int >;
        using HashType = std::unordered_map<KeyType, vector<int>, ss::SSHasher<KeyType > >;

    protected:
        const HashType &                           _index_map;
        ss::BucketSorter<DataType, KeyType> *      _sorter;

    public:
        ~CrossPolytopeRanker() { if(_sorter) delete _sorter; }

        //// for normal cross-polytope index
        explicit CrossPolytopeRanker(
                CrossPolytopeIndex<DataType> *     index,
                const DataType *                   query,
                const Metric<DataType > &          metric,
                const Matrix<DataType > &          data,
                const parameter &                  para)
                :
                Query<DataType >(index, query, metric, data, para), _index_map(index->getIndexMap()) {

            Initialize(index, query);
        }


        //// simple LSH using cross-polytope index
        using SimpleLSH = TransformLSHIndex<DataType, KeyType, CrossPolytopeIndex<DataType >, SimpleTransformer<DataType> >;
        explicit CrossPolytopeRanker(
                SimpleLSH *                        index,
                const DataType *                   query,
                const Metric<DataType > &          metric,
                const Matrix<DataType > &          data,
                const parameter &                  para)
                :
                Query<DataType >(index, query, metric, data, para), _index_map(index->getIndexMap()) {

            Initialize(index->GetIndex(), query);
        }

        //// sign ALSH using cross-polytope index
        using SignALSH = TransformLSHIndex<DataType, KeyType, CrossPolytopeIndex<DataType >, SignTransformer<DataType> >;
        explicit CrossPolytopeRanker(
                SignALSH *                         index,
                const DataType *                   query,
                const Metric<DataType > &          metric,
                const Matrix<DataType > &          data,
                const parameter &                  para)
                :
                Query<DataType >(index, query, metric, data, para), _index_map(index->getIndexMap()) {

            Initialize(index->GetIndex(), query);
        }

        void Initialize(CrossPolytopeIndex<DataType> * index, const DataType * query) {
            { /// construction here


                vector<vector<DataType > > xs = index->Rotate(query);
                vector<int >      max_indexes = index->ArgMaxAbs(xs);
                /// distance function from query to bucket(designated by index_key)
                auto distor = [&xs, &max_indexes] (const KeyType & index_key) {
                    DataType distance = 0.0f;
                    for (int k = 0; k < index_key.size(); ++k) {

                        DataType max_absolute = std::abs(xs[k][max_indexes[k]]);
                        DataType query_value = xs[k][ std::abs(index_key[k]) - 1 ]; ///index begin with 0
                        /// the cost make absolute value of query[index_key[k]] bigger than actual 'max_absolute'
                        distance += max_absolute - query_value * (index_key[k]>0?1:-1);
                    }
                    return distance;
                };
                /// sorter helper
                _sorter = new BucketSorter<DataType, KeyType>(_index_map, distor);
            } /// end construction
        }


        const vector<int > & NextBucket() override {
            HashType::const_iterator bucket;
            while ( (bucket = _index_map.find(_sorter->NextBucket()) ) ==_index_map.end()) {}
            return bucket->second;
        }

        bool NextBucketExisted() const override   {
            return _sorter->NextBucketExisted();
        }
    
    };

} // namespace ss
