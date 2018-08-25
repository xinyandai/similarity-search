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

#include <assert.h>

#include <cmath>
#include "utils/calculator.hpp"

namespace ss {

    template <typename DataType>
    class Metric {
    private:
        int  dim_;
    public:
        explicit Metric(int dim): dim_(dim) {}

        int dim() const {
            return dim_;
        }

        virtual float dist(const DataType *vec1, const DataType *vec2) const  = 0;
    };

    template <typename  DataType>
    class EuclidMetric : public Metric<DataType > {

    public:
        explicit EuclidMetric(int dim):Metric<DataType >(dim) {}

        float dist(const DataType *vec1, const DataType *vec2) const override {

            return ss::EuclidDistance(vec1, vec2, this->dim());
        }

    };

    template <typename  DataType>
    class IPDistance : public Metric<DataType > {

    public:
        explicit IPDistance(int dim):Metric<DataType >(dim) {}

        float dist(const DataType *vec1, const DataType *vec2) const override {

            return ss::InnerProductDistance(vec1, vec2, this->dim());
        }

    };

    template <typename  DataType>
    class AngularMetric : public Metric<DataType > {

    public:
        explicit AngularMetric(int dim):Metric<DataType >(dim) {}

        float dist(const DataType *vec1, const DataType *vec2) const override {

            return ss::AngularDistance(vec1, vec2, this->dim());
        }

    };

} // namespace ss