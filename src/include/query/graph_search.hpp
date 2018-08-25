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

#include <limits>
#include <vector>

#include "sorter/radix_sorter.hpp"

#include "../parameters.hpp"
#include "../query.hpp"
#include "../index/graph.hpp"
#include "../utils/calculator.hpp"

namespace ss {

    using std::vector;
    using std::pair;

    template <class DataType>
    class GraphSearch : public Query<DataType > {

    protected:
        typedef MaxHeapElement<int> Neighbor;
        const vector<vector<Neighbor > > & _graph;
        vector<bool >                      _visited;
        int                                _current;
    public:
        ~GraphSearch() {}

        explicit GraphSearch(
                GraphIndex<DataType > *            index,
                const DataType *                   query,
                const Metric<DataType > &          metric,
                const Matrix<DataType > &          data,
                const parameter &                  para)
                :
                Query<DataType >(index, query, metric, data, para),
                _graph(index->GetGraph()),
                _visited(data.getSize(), false) {}

        /**
         * Fast Approximate Nearest-Neighbor Search with k-Nearest Neighbor Graph
         * @param num_items
         */
        virtual void ProbeItems(const int num_items) {

            while(this->GetNumItemsProbed() < num_items && this->NextBucketExisted()) {      /// loop R

                _current = rand() % _graph.size();                               /// random generate Y_0(_current)
                for (int i = 0; i < 100; ++i) {                                   /// loop T

                    const vector<Neighbor> & bucket = _graph[_current];          /// N(Y, E, G), E=k here
                    float distance = std::numeric_limits<float >::max();
                    for(int i=0; i<bucket.size(); i++) {                         /// E=k=bucket.size() here
                        int id = bucket[i].data();
                        if (!_visited[id]) {                                     /// using a array to record
                            std::pair<float, bool > result = this->_scanner(id); /// update S and U
                            _visited[id] = true;

                            if (result.first < distance) {                       /// replace current Y
                                distance = result.first;
                                _current = id;
                            }
                        }

                    }
                }
            }
        }

        const vector<int > & NextBucket() override {
            assert(false);
        }

    };

} // namespace ss
