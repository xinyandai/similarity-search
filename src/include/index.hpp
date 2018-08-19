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

#include "matrix.h"
#include "parameters.hpp"

#include "utils/calculator.hpp"

namespace ss {

    /**
     * A interface of index structure which hold all base data.
     * Specific Search method is provide in "include/query/" and they implement the interface of "include/query.hpp"
     */
    template <class DataType>
    class Index {
    
    protected:
        const parameter & _para;
    public:
        explicit Index(const parameter& para): _para(para) {}
    
        /**
         * For Locality Sensitive Hashing, random vectors will be generated for projection and the training data won't be used
         * For Learning to Hashing, hashing functions will be learned
         * For Graph Method, nothing will happen.
         */    
        virtual void Train(const lshbox::Matrix<DataType> &) {}

        /**
         * all base data will be stored in this index structure.
         */
        virtual void Add(const lshbox::Matrix<DataType> &) {}

        virtual ~Index() {}
    };
}

