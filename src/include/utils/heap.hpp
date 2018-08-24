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
#include <utility>

using namespace std;

template<typename DataT, typename DistType=float>
class Distance {
protected:
    DistType    _dist;
    DataT       _data;

public:
    Distance(DistType dist, const DataT& data): _dist(dist), _data(data) {}

    explicit Distance(const pair<DistType, DataT>& p): _dist(p.first), _data(p.second) {}


    DistType dist() const {
        return _dist;
    }

    const DataT& data() const {
        return _data;
    }
};

// min heap element
template<typename DataT, typename DistType=float>
class MinHeapElement : public Distance<DataT> {
public:
    MinHeapElement(DistType dist, const DataT& data) : Distance<DataT>(dist, data) {}

    bool operator<(const MinHeapElement& other) const  {
        return this->_dist > other._dist;
    }
};

/// max heap element
template<typename DataT, typename DistType=float>
class MaxHeapElement : public Distance<DataT, DistType> {
public:
    MaxHeapElement(DistType dist, const DataT& data) : Distance<DataT>(dist, data) {}

    bool operator<(const MaxHeapElement& other) const  {
        return this->_dist < other._dist;
    }
};

/// TODO(Xinyan) : use std::vector and std::make_heap to replace priority queue
template <
        typename DataT,
        typename DistType=float,
        typename HeapElement=MaxHeapElement<DataT, DistType>
>
class Heap {

private:
    int                          _K;
    priority_queue<HeapElement > _heap;
public:
    Heap(int K) {
        _K = K;
    }

    void Insert(const HeapElement& pair) {
        if (_heap.size() < _K)
            _heap.push(pair);
        else {
            if (pair.dist() < _heap.top().dist()) {
                _heap.pop();
                _heap.push(pair);
            }
        }
    }

    void Insert(DistType dist, DataT data) {
        Insert(HeapElement(dist, data));
    }


    vector<HeapElement> GetTopK() const {
        priority_queue<HeapElement > heap = _heap;
        vector<HeapElement> results;

        while (!heap.empty()) {
            results.push_back(heap.top());
            heap.pop();
        }
        std::reverse(results.begin(), results.end());
        return results;
    }

    int getK() const {
        return _K;
    }

};

