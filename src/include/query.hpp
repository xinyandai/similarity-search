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

#include <utility>
#include <vector>

#include "parameters.hpp"
#include "index.hpp"
#include "metric.hpp"

#include "utils/topk.hpp"
#include "utils/calculator.hpp"

namespace ss {


    template <class DataType>
    class Query  {

    protected:
        Index<DataType >*                _index;
        Scanner<DataType>                _scanner;
        const parameter &                _para;
        const DataType *                 _query;
    public:
        Query(Index<DataType > *          index,
              const DataType *            query,
              const Metric<DataType > &   metric,
              const Matrix<DataType > &   data,
              const parameter &           para)
            :
            _index(index), 
            _query(query),
            _scanner(data, metric, query, para.topK),
            _para(para) {}

        virtual ~Query() {}
        

        virtual void ProbeItems(const int num_items) {

            while(GetNumItemsProbed() < num_items && NextBucketExisted()) {

                const vector<int>& bucket = NextBucket();
                for(int i=0; i<bucket.size(); i++)
                    // _scanner.operator() do two things:
                    // 1. calculate the true distance between query and bucekt[i] 
                    // 2. try to put bucket[i] into topK(heap)
                    _scanner(bucket[i]); 
            }
        }

        /**
         * return the next bucket should be visited/probed.
         * should be implement by specific class, such as Hamming Ranking for BitIndex, Int Ranking for IntIndex...
         */
        virtual const vector<int> & NextBucket() = 0;

        virtual bool NextBucketExisted() const {
            return GetNumItemsProbed() < _para.base_size;
        }

        inline int GetNumItemsProbed() const {
            return _scanner.count();
        }

        inline vector<pair<float, int> >  GetSortedTopK() const {
            return  _scanner.TopKPairs();
        }
    };

} // namespace ss
