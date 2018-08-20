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

#include "sorter/linear_sorter.hpp"

#include "../parameters.hpp"
#include "../query.hpp"
#include "../index/kmeans.hpp"
#include "../utils/calculator.hpp"

namespace ss {

    using namespace lshbox;

    template <class DataType>
    class ClusterRanker : public Query<DataType > {

        using AccessorType = typename lshbox::Matrix<DataType>::Accessor;
        using KeyType      = const DataType *;

    protected:
        const vector<vector<DataType > > &   _centers;
        const vector<vector<int > >      &   _points;;
        LinearSorter<DataType> *             _sorter;

    public:
        ~ClusterRanker() { if(_sorter) delete _sorter; }

        explicit ClusterRanker(
                KMeansIndex<DataType > *     index,
                DataType *                   query,
                lshbox::Metric<DataType > &  metric,
                const AccessorType &         accessor,
                const parameter &            para)
                :
                Query<DataType >(index, query, metric, accessor, para),
                _centers(index->get_centers()),
                _points(index->get_points()) {

            auto distor = [query, this, &para] (int center) {
                return ss::EuclidDistance(query, this->_centers[center].data(), para.dim);
            };
            _sorter = new LinearSorter<DataType>(_centers.size(), distor);
        }

        const vector<int >& NextBucket() override { return _points[_sorter->NextBucket()]; }

        bool NextBucketExisted() const override   { return _sorter->NextBucketExisted(); }

    };
}
