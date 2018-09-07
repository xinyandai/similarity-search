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
#include <algorithm>
#include <cmath>

#include "../../utils/calculator.hpp"

#include "../../index.hpp"
#include "../srp.hpp"

#include "simple_transformer.hpp"

namespace ss {

    using std::vector;

    template<class DataType, class KeyType, class HashingIndexType, class Transformer=ss::SimpleTransformer<DataType > >
    class TransformLSHIndex : public MapIndex<DataType, KeyType > {

    protected:
        HashingIndexType *                   _index;
        std::vector<DataType>                _norms;
        DataType                             _max_norm;
        Transformer                          _transformer;
    public:
        ~TransformLSHIndex() {
            delete _index; 
        }

        explicit TransformLSHIndex(const parameter& para) :
            MapIndex<DataType, KeyType >(para),
            _index(new HashingIndexType(para)),
            _norms(),
            _max_norm(0),
            _transformer(para) {}

        HashingIndexType * GetIndex() {
            return _index;
        }

        void Train(const Matrix<DataType >& data) override {

            _norms = data.calNorms();
            _max_norm = *std::max_element(std::begin(_norms), std::end(_norms));

            _index->Train(data);
        }


        KeyType HashData(const DataType *data, int id) override {

            const DataType * transformed_data = _transformer.TransformData(data, _norms[id], _max_norm);
            return _index->HashData(transformed_data, id);
        }

        KeyType HashQuery(const DataType *data, int id) override {

            const DataType * transformed_query = _transformer.TransformQuery(data);
            return _index->HashQuery(transformed_query, id);
        }

    protected:
        KeyType Quantize(const DataType * data)  override {std::cerr << "should never be called" << std::endl;}
    };

} // namespace ss
