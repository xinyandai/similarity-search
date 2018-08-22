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
#include <vector>
#include <algorithm>
#include <utility>
#include <queue>
#include <functional>
#include "../../utils/heap.hpp"
using std::vector;
using std::pair;
using std::function;
using std::priority_queue;

template<typename DataT>
DataT vector_product(const vector<DataT > & data_vector) {
    DataT point_size = 1;
    for (size_t i = 0; i < data_vector.size(); ++i) {
        point_size *= data_vector[i];
    }
    return point_size;
}

// stands for inverted multi index
class IMISequence {

    typedef vector<int > Coord;

protected:
    // dist stores float value, data stores the index in the given leftVec or rightVec;
    vector<size_t >                         _dimensions;
    vector<bool >                           _visited;
    function<float (Coord)>                 _distor;
    priority_queue<MinHeapElement<Coord > > _minHeap;

public:

    IMISequence(const vector<size_t > & dimensions, const function<float (Coord)>& func) :
            _dimensions(dimensions), _distor(func) {

        EnHeap(vector<int >(_dimensions.size(), 0));
        _visited = vector<bool >(vector_product(_dimensions), false);

    }


    bool HasNext() const {
        return !_minHeap.empty();
    }


    /**
     * time complexity O(_dimension*_dimension*_dimension)
     * @return
     */
    pair<float, Coord > Next() {

        const Coord nearestInHeap = _minHeap.top().data();
        float dist = _minHeap.top().dist();
        _minHeap.pop();

        _visited[Position(nearestInHeap)] = true;

        for (int i = 0; i < _dimensions.size(); ++i) {
            // check the next element in (i+1)'th dimension
            int new_index = nearestInHeap[i] + 1;

            if(new_index  < _dimensions[i]) { // next element exists

                Coord nextEnHeap = nearestInHeap;
                nextEnHeap[i] = new_index;

                if (ShouldEnHeap(nextEnHeap)) {
                    EnHeap(nextEnHeap);
                }
            }
        }

        return std::make_pair(dist, nearestInHeap);
    }


    void EnHeap(const Coord &coord) {
        float dist = _distor(coord);
        _minHeap.emplace(MinHeapElement<Coord>(dist, coord));
    }

protected:

    /***
     * @param coord
     * @return position in visited_ array
     */
    inline int Position(const Coord &coord) {
        int index = coord[0];
        for (int i = 1; i < _dimensions.size(); ++i) {
            index *= _dimensions[i-1];
            index += coord[i];
        }
        return index;
    }

    /**
     * time complexity O(_dimension)
     * @param coord
     * @return coord is already visited
     */
    inline bool Visited(const Coord &coord) {
        return _visited[Position(coord)];
    }

    /**
     * time complexity O(_dimension*_dimension)
     * @param coord
     * @return if all point before coord in any dimension is visited
     */
    inline bool ShouldEnHeap(const Coord &coord) {

        for (int i = 0; i < _dimensions.size(); ++i) {
            if (coord[i]>=1) { // preCoord existed

                Coord preOne = coord;
                preOne[i]--;
                if (!Visited(preOne)) {
                    return false;
                }
            }
        }

        return true;
    }

};

