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

#include <unordered_map>
#include <utility>
#include <vector>
#include <limits>

#include "utils/calculator.hpp"
#include "utils/hashers.hpp"

namespace ss {

    using std::vector;
    using std::pair;
    using std::unordered_map;

    template<class DataType, class KeyType>
    class SubDataSorter {
    protected:
        vector<vector<pair<DataType, KeyType > > > _sorted_bucket;
        vector<int >                               _index ;
    public:
        /// TODO(Xinyan): remove the hash function
        explicit SubDataSorter(
                const unordered_map<KeyType, vector<int>, ss::SSHasher<KeyType > > & map,
                const std::function<DataType (const KeyType&)> &                     inner_distor,
                const std::function<DataType (const KeyType&)> &                     inter_distor,
                int num_data)
                :
                _sorted_bucket(num_data), _index(num_data) {

            for(auto it = map.begin(); it!=map.end(); it++) {
                DataType distance = inner_distor(it->first);
                _sorted_bucket[GetSubDataSet(it->first)].emplace_back(std::make_pair(distance, it->first ) );
            }

            for (int i = 0; i < num_data; ++i) {
                ss::SortPairByFirst(&_sorted_bucket[i]);
            }

            for (int i = 0; i < num_data; ++i) {
                for (int j = 0; j < _sorted_bucket[i].size(); ++j) {
                    _sorted_bucket[i][j].first = inter_distor(_sorted_bucket[i][j].second);
                }
            }
        }


        int GetSubDataSet(vector<int > key) {
            return key[key.size()-1];
        }

        virtual KeyType NextBucket() {
            int min_idx = -1;
            DataType min = std::numeric_limits<DataType>::max();
            for (int i = 0; i < _index.size(); ++i) {
                if (_index[i]<_sorted_bucket[i].size() && _sorted_bucket[i][_index[i]].first < min) {
                    min = _sorted_bucket[i][_index[i]].first;
                    min_idx = i;
                }
            }
            assert(min_idx>=0);
            return _sorted_bucket[min_idx][_index[min_idx]++].second;
        }

    };

} // namespace ss
