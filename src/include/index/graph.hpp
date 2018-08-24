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
#include <boost/progress.hpp>

#include "../index.hpp"
#include "utils/ground_truth.hpp"

namespace ss {

    using std::vector;

    template<class DataType>
    class GraphIndex : public Index<DataType> {
        typedef MaxHeapElement<int> Neighbor;
        vector<vector<Neighbor > > _graph;
    public:
        explicit GraphIndex(const parameter& para) : Index<DataType >(para) {}

        void Train(const Matrix<DataType > & data) override {
            /// do nothing here
        }

        void Add(const Matrix<DataType > & data) override {
            ///
            _graph = ss::ExactKNN<DataType>(
                    data[0],
                    data.getSize(),
                    data[0],
                    data.getSize(),
                    data.getDim(),
                    this->_para.graph_K,
                    ss::EuclidDistance<DataType >
            );
        }

        const vector<vector<Neighbor > >& GetGraph() {
            return _graph;
        }

    };

} // namespace ss
