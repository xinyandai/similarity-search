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

#include <unordered_map>
#include <vector>

#include "../index.hpp"

#include "../utils/hashers.hpp"

namespace ss {

    using namespace std;

    template<class DataType, class KeyType >
    class MapIndex : public Index<DataType> {


    protected:
        /**
         * if you want to use means in each dimension, then you should invoke #initilize_mean() in #train();
         * if not, let it alone and means are initilized with default value 0.
         */
        vector<DataType >                                            _means;
        unordered_map<KeyType, vector<int>, ss::SSHasher<KeyType > > _hash_map;
        vector<vector<DataType > >                                   _projectors;
    public:
        explicit MapIndex(const parameter& para) :
            Index<DataType >(para),
            _means(para.dim, 0),
            _projectors(para.num_bit, vector<DataType >(para.dim, 0)) {}

        unordered_map<KeyType, vector<int>, SSHasher<KeyType > > & getIndexMap() { return _hash_map;  }

        virtual void Train(const lshbox::Matrix<DataType> &) =0 ;

        void Add(const lshbox::Matrix<DataType> &data) override {

            for (int i=0; i<data.getSize(); ++i) {
                KeyType hash_val = HashData(data[i], i);
                _hash_map[hash_val].push_back(i);
            }
        }

        virtual KeyType HashData(const DataType *data, int id)   { return this->Quantize(data); }
        virtual KeyType HashQuery(const DataType *query, int id) { return this->HashData(query, -1); }

    protected:

        virtual KeyType Quantize(const DataType *data) = 0;

        void InitializeMeans(const lshbox::Matrix<DataType> &train_data) {

            vector<long double> sum(this->_para.dim, .0);    // use long double to make sure presision
            for (int i=0; i < train_data.getSize(); i++) {    // one vector by one vector cache-friendly
                for( int d = 0; d < this->_para.dim; d++ ) {
                    sum[d] += train_data[i][d] ;
                }
            }
            for (int d = 0; d < this->_para.dim; d++) {
                this->_means[d]  = sum[d] / train_data.getSize();
            }
        }

    };

}
