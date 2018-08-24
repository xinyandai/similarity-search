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
#include <utility>
#include <unordered_map>

#include "sorter/imi_sorter.hpp"

#include "../parameters.hpp"
#include "../query.hpp"
#include "../index/pq.hpp"
#include "../utils/calculator.hpp"

namespace ss {

    using  std::unordered_map;

    template <class DataType>
    class IMIProber : public Query<DataType > {

        using HashTableType = unordered_map< vector<int >, vector<int>, ss::SSHasher< vector<int > > >;

    protected:

        IMISequence *                              _imi_sequence;
        vector<vector<std::pair<DataType, int> > > _dist_to_centers;
        HashTableType &                            _hash_map;
    public:

        virtual ~ IMIProber() {
            if (_imi_sequence) delete _imi_sequence;
        }

        explicit IMIProber(
                PQIndex<DataType > *               index,
                const DataType *                   query,
                const Metric<DataType > &          metric,
                const Matrix<DataType > &          data,
                const parameter &                  para)
                :
                Query<DataType >(index, query, metric, data, para),
                _dist_to_centers(index->DistToCenters(query)),
                _hash_map(index->hash_map()) {

            { /// construction here

                /// sorting all centers by ascending order, in each code-book
                for (int i = 0; i < _dist_to_centers.size(); ++i) {
                    ss::SortPairByFirst(&_dist_to_centers[i]);
                }
                /// distance function from query to bucket(designated by coord)
                auto distor = [this, &para](vector<int > coord) {
                    DataType distance = 0.0f;
                    for (int code_book = 0; code_book < para.num_codebook; ++code_book) {
                        distance += _dist_to_centers[code_book][coord[code_book]].first;
                    }
                    return distance;
                };
                /// sorting helper
                _imi_sequence = new IMISequence(vector<size_t >(para.num_codebook, para.kmeans_centers ), distor);

            } /// end construction
        }

        const vector<int > & NextBucket() override {

            vector<int > coord = _imi_sequence->Next().second;
            vector<int > clusters(coord.size());
            for (int code_book = 0; code_book < coord.size(); ++code_book) {
                clusters[code_book] = _dist_to_centers[code_book][coord[code_book]].second;
            }
            return _hash_map[clusters];
        }

        bool NextBucketExisted() const override   {
            return _imi_sequence->HasNext();
        }

    };

} // namespace ss
