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

#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>

#include "../../utils/calculator.hpp"

#include "../map_index.hpp"
#include "../srp.hpp"

#include "simple_transformer.hpp"

namespace ss {

    using std::vector;

    template<
            class DataType,
            class KeyType=uint64_t,
            class HashingIndexType=SRPIndex<DataType >,
            class Transformer=ss::SimpleTransformer<DataType >
    >
    class NormRangeIndex : public MapIndex<DataType, KeyType > {
    protected:
        HashingIndexType *              _index;
        const int                       _num_sub_data_set;
        const int                       _bit_sub_data_set;
        std::vector<DataType>           _norms;
        std::vector<DataType>           _percentiles;
        Transformer                     _transformer;
    public:
        ~NormRangeIndex() { 
            delete _index; 
        }

        explicit NormRangeIndex(const parameter& para) :
                MapIndex<DataType, KeyType >(para),
                _index(new HashingIndexType(para)),
                _transformer(para),
                _num_sub_data_set(para.num_sub_data_set),
                _bit_sub_data_set(static_cast<int>(std::ceil(std::log2(_num_sub_data_set)))) {}

        void Train(const Matrix<DataType> &data) override {
            _norms = data.calNorms(); /// TODO(Xinyan: ): should be improved
            InitializePercentile();
            _index->Train(data);
        }


        HashingIndexType * GetIndex() {
            return _index;
        }


        /**
         * returned Key is composed of hash_value and index of sud-data-set
         * @param data pointer to data vector
         * @param id the id of data
         * @return
         */
        KeyType HashData(const DataType *data, int id) override {
            int sub_data_set = GetSubDataSet(id);
            DataType max_norm = _percentiles[sub_data_set];
            const DataType * transformed_data = _transformer.TransformData(data, _norms[id], max_norm);
            KeyType hash_value = _index->HashData(transformed_data, id);
            this->MergeBucketKey(hash_value, sub_data_set);
            return hash_value;
        }

        /**
         * returned key is equal to hash values, because the query's norm is not important
         * @param data pointer to query vector
         * @param id query's id
         * @return
         */
        KeyType HashQuery(const DataType *data, int id) override {

            const DataType * transformed_query = _transformer.TransformQuery(data);
            return _index->HashQuery(transformed_query, id);
        }

        /**
         * determine which sub-data-set the data belongs to
         * @postcondition make sure that _percentiles[index-1]<_norms[id]<=_percentiles[index]
         * @param id
         * @return index of sub-data-set
         */
        int GetSubDataSet(int id) { /// TODO(Xinyan): should avoid linear scan.
            assert(id>=0 && id<_norms.size());
            DataType norm = _norms[id];
            int i = 0;
            while(i < _percentiles.size() && _percentiles[i++] <  norm) {}
            // if i==_percentiles.size() , we use i-1; if _percentiles[i]<=norm we use i (we use 'i-1' because of 'i++')
            assert(_percentiles[i-1]>=norm);
            if(i>1) assert(_percentiles[i-2]<norm);
            return i-1;
        }


        /**
         * retrieve two things as follow:
         * 1. the hash(the number of different hash value) distance between query's hash values and
         * bucket_key(composed of hash_value and index of sud-data-set)
         * 2. sub-data-set's maximum norm, aka, scale factor
         * @param bucket_key
         * @param query_hash
         * @return
         */
        std::pair<int, DataType> HashDistAndPercentile(const KeyType &bucket_key, const KeyType &query_hash) {
            return std::make_pair(HashDist(bucket_key, query_hash), Percentile(bucket_key));
        }

        int HashDist(const uint64_t &bucket_key, const uint64_t &query_hash) {
            return ss::CountBitOne(query_hash ^ (bucket_key >> _bit_sub_data_set));
        }

        int HashDist(const vector<int > &bucket_key, const vector<int > &query_hash) {
            return ss::CountDiff(query_hash.data(),  bucket_key.data(), this->_para.num_bit);
        }

        DataType Percentile(const uint64_t &bucket_key) {
            static KeyType mask = (1<<_bit_sub_data_set) - 1; //2^i - 1 == 000000111111(i's 1 in low bits)
            int sub_data_set = bucket_key & mask;
            return _percentiles[sub_data_set];
        }

        DataType Percentile(const vector<int > &bucket_key) {
            int sub_data_set = bucket_key[bucket_key.size()-1];
            return _percentiles[sub_data_set];
        }

        /**
         * we choose to merge hash value and index of sub-data-set into one key, aka, bucket-key
         * @param hash_val
         * @param sub_data_set
         * @return
         */
        void MergeBucketKey(uint64_t &hash_val, int sub_data_set) {
            hash_val = (hash_val << _bit_sub_data_set) | sub_data_set;
        }
        void MergeBucketKey(vector<int > &hash_val, int sub_data_set) {
            hash_val.push_back(sub_data_set);
        }

    protected:

        KeyType Quantize(const DataType *data) override {std::cerr << "Should never be called." << std::endl; };

        void InitializePercentile() {
            std::vector<DataType > norms = _norms;
            std::sort(norms.begin(), norms.end());
            
            _percentiles.resize(_num_sub_data_set);
            for (int i=0; i<_num_sub_data_set; i++) {
                _percentiles[i] = norms[ norms.size() / _num_sub_data_set * (i+1) + norms.size() % _num_sub_data_set -1 ];
            }

        }

        void UniformPercentile() {
            DataType min = *std::min_element(std::begin(_norms), std::end(_norms));
            DataType max = *std::max_element(std::begin(_norms), std::end(_norms));
            _percentiles.resize(_num_sub_data_set);
            for (int i=0; i<_num_sub_data_set; i++) {
                _percentiles[i] = min + ((i+1.0) / _num_sub_data_set) * (max - min);
            }
        }

    };

} // namespace ss
