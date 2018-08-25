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

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>
#include <utility>

namespace ss {

    using std::vector;
    using std::pair;

    template <typename FirstType, typename SencondType>
    void SortPairByFirst(vector<std::pair<FirstType, SencondType> > * _sorted_bucket) {
        std::sort(
                _sorted_bucket->begin(),
                _sorted_bucket->end(),
                [](const pair<FirstType, SencondType >& a, const pair<FirstType, SencondType>& b) {
                    if (a.first != b.first)
                        return a.first < b.first;
                    else
                        return a.second < b.second;
                });
    }

    template <typename FirstType, typename SencondType>
    void SortPairBySecond(vector<std::pair<FirstType, SencondType> > * _sorted_bucket) {
        std::sort(
                _sorted_bucket->begin(),
                _sorted_bucket->end(),
                [](const pair<FirstType, SencondType >& a, const pair<FirstType, SencondType>& b) {
                    if (a.second != b.second)
                        return a.second < b.second;
                    else
                        return a.first < b.first;
                });
    }


    template <class DataType >
    DataType inline InnerProduct(const DataType *a, const DataType *b, int dim) {

        DataType sum = .0;
        for (int i = 0; i < dim; ++i) {
            sum += a[i] * b[i];
        }
        return sum;
    }

    template <class DataType>
    DataType inline CalculateNorm(const DataType *a, int dim) {
        return std::sqrt(InnerProduct(a, a, dim));
    }


    template <class DataType >
    DataType inline Cosine(const DataType *a, const DataType *b, int dim) {
        return InnerProduct(a, b, dim) / static_cast<double >(CalculateNorm(a, dim)) / CalculateNorm(b, dim);
    }


    template <class DataType>
    DataType inline DiffProduct(const DataType *a, const DataType *means, const DataType *b, int dim) {
    
        DataType sum = .0;
        for (int i = 0; i < dim; ++i) {
            sum += (a[i] - means[i]) * b[i];
        }
        return sum;
    }


    template <class DataType >
    DataType inline EuclidDistance(const DataType *a, const DataType *b, int dim) {

        DataType sum = .0;
        for (int i = 0; i < dim; ++i) {
            sum += (a[i] - b[i]) * (a[i] - b[i]);
        }
        return sqrt(sum);
    }

    template <class DataType >
    DataType inline AngularDistance(const DataType *a, const DataType *b, int dim) {
        return std::acos(Cosine(a, b, dim));
    }

    template <class DataType >
    DataType inline InnerProductDistance(const DataType *a, const DataType *b, int dim) {
        return - InnerProduct(a, b, dim);
    }


    template <class DataType>
    void inline ScaleData(DataType *target, const DataType *data, DataType scale, int dim) {
        for (int d = 0; d<dim; d++) {
            target[d] = data[d] / scale;
        }
    }

    template <class DataType>
    void inline SimpleLSHTransform(DataType *transform_buffer, const DataType *data, DataType norm, DataType scale,
                                   int origin_dim) {
        
        if (scale < norm) {
            std::cerr <<"the vector's scale: "<< scale <<" is smaller than norm: " << norm <<std:: endl;
            scale = norm;
        }
        ss::ScaleData(transform_buffer, data, scale, origin_dim);
        transform_buffer[origin_dim] = std::sqrt(1 - norm*norm/scale/scale);
    }

    template <class DataType>
    void inline SimpleLSHTransform(DataType *transform_buffer, const DataType *data, int origin_dim) {
        DataType norm = ss::CalculateNorm(data, origin_dim);
        ss::ScaleData(transform_buffer, data, norm, origin_dim);
        transform_buffer[origin_dim] = 0.0;
    }

    int inline CountBitOne(uint64_t xorval) {
        int count = 0;

        while(xorval != 0) {
            count++;
            xorval &= (xorval-1);
        }
        return count;
    }

    template <class DataType >
    int inline CountDiff(const DataType *a, const DataType *b, int dim) {
        int count = 0;
        for (int d=0; d<dim; d++) {
            if (a[d]!=b[d]) {
                count++;
            }
        }
        return count;
    }

} // namespace ss
