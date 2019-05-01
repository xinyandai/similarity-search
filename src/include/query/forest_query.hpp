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
/// all copies or substantial portions of the Software.
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

#include <bitset>
#include <functional>
#include <vector>

#include "sorter/radix_sorter.hpp"

#include "../parameters.hpp"
#include "../query.hpp"
#include "../index.hpp"

namespace ss {


    template <class DataType>
    class ForestQuery : public Query<DataType > {
    protected:
        vector<bool >                                    _visited;
    public:
        ~ForestQuery() { }

        explicit ForestQuery(
                Index<DataType >                 *index,
                const DataType                   *query,
                const Metric<DataType >          &metric,
                const Matrix<DataType >          &data,
                const parameter                  &para):
                Query<DataType >(index, query, metric, data, para),
                _visited(data.getSize(), false) {}

        void ProbeItems(const int num_items) {
            this->_index->Search(this->_query, [&](int id) {this->RepeatProbe(id); });
        }

        void RepeatProbe(int id) {
            if (! _visited[id]) {
                _visited[id] = true;
                this->probe(id);
            }
        }

        const vector<int> & NextBucket() override {}
    };

} // namespace ss
