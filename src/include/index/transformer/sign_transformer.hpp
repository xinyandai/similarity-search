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

namespace ss {

    using std::vector;

    template<class DataType = float>
    class SignTransformer {

        vector<DataType> _transformed_data;
        const parameter &_para;
    public:

        explicit SignTransformer(const parameter &para) : _transformed_data(para.dim), _para(para) {}

        const DataType *TransformData(const DataType *data, DataType norm, DataType max_norm) {

            DataType scale = max_norm / 0.75f; /// hard code is perfect, check the paper you self

            ss::ScaleData(_transformed_data.data(), data, scale, this->_para.origin_dim);
            for (int i = 0; i < this->_para.transformed_dim; ++i) {
                _transformed_data[this->_para.origin_dim + i] =
                        0.5f - std::pow(norm / scale, 1 << (i + 1)); /// 1/2 - |x|^(2^m)
            }

            return _transformed_data.data();
        }

        const DataType *TransformQuery(const DataType *data) {
            DataType norm = ss::CalculateNorm(data, this->_para.origin_dim);
            ss::ScaleData(_transformed_data.data(), data, norm, this->_para.origin_dim);
            for (int i = 0; i < this->_para.transformed_dim; ++i) {
                _transformed_data[this->_para.origin_dim + i] = 0.0;
            }
            return _transformed_data.data();
        }

    };

} // namespace ss
