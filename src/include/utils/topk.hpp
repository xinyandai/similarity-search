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
#include <iostream>
#include <algorithm>
#include <utility>
#include <unordered_set>

#include "matrix.hpp"
#include "metric.hpp"

#include "utils/heap.hpp"

using std::unordered_set;
using std::pair;
using std::vector;
namespace ss {

    template <typename DataType>
    class Scanner {

    private:
        Metric<DataType>            _metric;
        Heap<int, float>            _heap;
        int                         _K;
        int                         _count;
        const Matrix<DataType > &   _data;
        const DataType *            _query;

    public:
        Scanner(const Matrix<DataType> & data,
                const Metric<DataType> & metric,
                const DataType *         query,
                int                      K)
                :
                _data(data), _metric(metric), _K(K), _count(0), _query(query), _heap(K) {}


        int count() const {
            return _count;
        }


        int getK() {
            return _K;
        }


        vector<pair<float, int > > TopKPairs() const {
            return _heap.TopKPairs();
        }

        /**
         * Update the current query by scanning key, this is normally invoked by the LSH
         * index structure.
         */
        void operator () (int id) {
            ++_count;
            _heap.Insert(CalDist(id), id);
        }


        float CalDist(int id) const {
            return _metric.dist(_query, _data[id]);
        }

    };

} // namespace ss
