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

#include <utility>
#include <vector>
#include <random>
#include <unordered_map>
#include <boost/progress.hpp>

#include "../index.hpp"

namespace ss {

    using std::vector;

    template<class DataType, class IndexType>
    class ForestIndex : public Index<DataType> {
    protected:
        vector<IndexType* > _indices;
    public:
        explicit ForestIndex(const parameter &para) :
                Index<DataType>(para), _indices(para.forest_size) {

            for (int i = 0; i < this->_para.forest_size; ++i) {
                _indices[i] = new IndexType(para);
            }
        }

        void Train(const Matrix<DataType> &data) override {
            for (int i = 0; i < this->_para.forest_size; ++i) {
                _indices[i]->Train(data);
            }
        }


        void Add(const Matrix<DataType> &data) override {
            for (int i = 0; i < this->_para.forest_size; ++i) {
                _indices[i]->Add(data);
            }
        }

        void Search(const DataType *query, const std::function<void (int)>& prober) override {
            for (int i = 0; i < this->_para.forest_size; ++i) {
                _indices[i]->Search(query, prober);
            }
        }
    };

} // namespace ss
