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

#include <bitset>

#include "../../query.hpp"
#include "../../index/bit_index.hpp"
#include "../../utils/hashers.hpp"

namespace ss {

    using namespace lshbox;

    template <class DataType, class KeyType >
    class RadixSorter {
        using AccessorType = typename lshbox::Matrix<DataType>::Accessor;

    protected:
        vector<vector<KeyType > >  _dist_bucket;
        int                        _i;
        int                        _j;

    public:
        explicit RadixSorter(
                unordered_map<KeyType, vector<int>, SSHasher<KeyType > > &  map,
                const std::function<int (const KeyType&) > &                distor,
                int                                                         maximum_distance)
                :
                _dist_bucket(maximum_distance+1, vector<KeyType >()),
                _i(0),
                _j(0) {
            
            for(auto it = map.begin(); it != map.end(); it++) {
            
                int hamming_dist = distor(it->first);
                assert(hamming_dist>=0 && hamming_dist<=maximum_distance);
                _dist_bucket[hamming_dist].push_back(it->first);
                assert(it->second.size() > 0);
            }
        }


        virtual KeyType& NextBucket() {
            while(_j == _dist_bucket[_i].size()) {
                _j = 0;
                _i ++;
            }
            return _dist_bucket[_i][_j++];
        }

        virtual bool NextBucketExisted() {
            return _i < _dist_bucket.size();
        }
    
    };
}
