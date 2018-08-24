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

/**
 * @file metric.h
 *
 * @brief Common distance measures.
 */
#pragma once

#include <assert.h>

#include <cmath>

namespace ss
{
#define L1_DIST 1
#define L2_DIST 2
#define AG_DIST 3    // angular distance
#define IP_DIST 4    // angular distance
/**
 * The calculation of square.
 */
template <typename DATATYPE>
DATATYPE sqr(const DATATYPE &x)
{
    return x * x;
}
/**
 * Use for common distance functions.
 */
template <typename DATATYPE>
class Metric
{
    unsigned type_;
    unsigned dim_;
public:
    /**
     * Constructor for this class.
     *
     * @param dim  Dimension of each vector
     * @param type The way to measure the distance, you can choose 1(L1_DIST) or 2(L2_DIST)
     */
    Metric(unsigned dim, unsigned type): dim_(dim), type_(type) {}
    ~Metric() {}
    /**
     * Get the dimension of the vectors
     */
    unsigned dim() const
    {
        return dim_;
    }
    /**
     * measure the distance.
     *
     * @param  vec1 The first vector
     * @param  vec2 The second vector
     * @return      The distance
     */
    float dist(const DATATYPE *vec1, const DATATYPE *vec2) const
    {
        float dist_ = 0.0;
        switch (type_)
        {
        case L1_DIST:
        {
            for (unsigned i = 0; i != dim_; ++i)
            {
                dist_ +=  static_cast<float>( std::abs(vec1[i] * 1.0 - vec2[i]));
            }
            return dist_;
        }
        case L2_DIST:
        {
            for (unsigned i = 0; i != dim_; ++i)
            {
                dist_ += sqr(vec1[i] - vec2[i]);
            }
            return std::sqrt(dist_);
        }
        case AG_DIST:
        {
            float norm_1 = 0.0;
            float norm_2 = 0.0;
            for (unsigned i = 0; i != dim_; ++i)
            {
                dist_ += vec1[i] * vec2[i];
                norm_1 += sqr(vec1[i]);
                norm_2 += sqr(vec2[i]);
            }
            return acos( dist_ / std::sqrt(norm_1*norm_2) );
        }
        case IP_DIST:
        {
            for (unsigned i = 0; i != dim_; ++i)
            {
                dist_ += vec1[i] * vec2[i];
            }
            return - dist_;
        }

        default:
        {
            assert(false);
        }
        }
    }
};

} // namespace ss