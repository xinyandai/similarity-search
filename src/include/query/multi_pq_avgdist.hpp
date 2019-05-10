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
#include <unordered_map>
#include <algorithm>
#include <numeric>

#include "sorter/imi_sorter.hpp"

#include "../parameters.hpp"
#include "../query.hpp"
#include "../index/multi_pq.hpp"
#include "../utils/calculator.hpp"

namespace ss
{
template <class DataType>
class MultiPQAvgdist : public Query<DataType>
{
protected:
    IMISequence *_imi_sequence;
    MultiPQIndex<DataType> *_multi_pq;
    vector<unsigned int> _visited;
    vector<vector<std::pair<DataType, int>>> _dist_to_centers;
    vector<float> _avgdist;

public:
    ~MultiPQAvgdist()
    {
        if (_imi_sequence)
            delete _imi_sequence;
    }

    explicit MultiPQAvgdist(
        MultiPQIndex<DataType> *index,
        const DataType *query,
        const Metric<DataType> &metric,
        const Matrix<DataType> &data,
        const parameter &para)
        : Query<DataType>(index, query, metric, data, para),
          _multi_pq(index),
          _dist_to_centers(index->DistToCenters(query)),
          _visited(data.getSize(), 0),
          _avgdist(data.getSize(), 0.0)
    {

        /// sorting all centers by ascending order, in each code-book
        for (int i = 0; i < _dist_to_centers.size(); ++i)
        {
            ss::SortPairByFirst(&_dist_to_centers[i]);
        }
        /// distance function from query to bucket(designated by coord)
        auto distor = [this, &para](vector<int> coord) {
            DataType distance = 0.0f;
            for (int code_book = 0; code_book < 2; ++code_book)
            {
                // distance is equal to sum of distance in each codebook
                // first is distance, second is id
                distance += _dist_to_centers[code_book][coord[code_book]].first;
            }
            return distance;
        };
        _imi_sequence = new IMISequence(vector<size_t>(2, para.kmeans_centers * para.forest_size), distor);
    }

    void ProbeItems(const int num_items) override
    {
        while (this->GetNumItemsProbed() < num_items && NextBucketExisted())
        {

            pair<float, vector<int>> next_item = _imi_sequence->Next();
            vector<int> coord = next_item.second;
            float temp_dist = next_item.first;
            std::vector<int> bucket = _multi_pq->SearchByID(
                _dist_to_centers[0][coord[0]].second,
                _dist_to_centers[1][coord[1]].second);
            for (int id : bucket)
                ProbeRank(id, temp_dist);
        }
    }

    void ProbeRank(int id, float temp_dist)
    {
        if (!_visited[id])
        {
            this->_count++;
        }
        _visited[id]++;
        _avgdist[id] = (_avgdist[id] * (_visited[id] - 1) + temp_dist) / _visited[id];
    }

    vector<pair<float, int>> GetSortedTopK() const override
    {

        vector<pair<float, int>> unsorted;
        unsorted.reserve(_visited.size());
        for (int i = 0; i < _visited.size(); i++)
        {
            if (_avgdist[i] > 0.000001)
                unsorted.push_back(std::make_pair(_avgdist[i], i));
        }

        std::sort(unsorted.begin(), unsorted.end(), [](const pair<float, int> &i, const pair<float, int> &j) { return i.first < j.first; });

        vector<pair<float, int>> sorted(unsorted.begin(), unsorted.begin() + this._para.topK);
        return sorted;
    }

    const vector<int> &NextBucket() override
    {
        throw std::invalid_argument("Should not invoke this function!");
    }

    bool NextBucketExisted() const override
    {
        return _imi_sequence->HasNext();
    }
};
} // namespace ss
