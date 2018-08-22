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

#include "parameters.hpp"
#include "index.hpp"
#include "metric.h"

#include "utils/topk.h"
#include "utils/calculator.hpp"

namespace ss {

    using namespace lshbox;

    template <class DataType>
    class Query  {
        using AccessorType = typename lshbox::Matrix<DataType>::Accessor;
    
    private:
        Index<DataType >*                _index;
        DataType*                        _query;
        lshbox::Scanner<AccessorType >   _scanner;
        const parameter &                _para;

    public:
        Query(Index<DataType > *            index,
                DataType *                  query,
                lshbox::Metric<DataType > & metric,
                const AccessorType &        accessor,
                const parameter &           para)
            :
            _index(index), 
            _query(query), 
            _scanner(accessor, metric, para.topK), 
            _para(para) {
            
            _scanner.reset(query);
        }

        virtual ~Query() {}
        
        /**
         * 
         */    
        virtual void ProbeItems(const int num_items) {

            while(GetNumItemsProbed() < num_items && NextBucketExisted()) {

                const vector<int>& bucket = NextBucket();
//                assert(bucket.size() > 0);

                for(int i=0; i<bucket.size(); i++)
                    // _scanner.operator() do three things:
                    // 1. calculate the true distance between query and bucekt[i] 
                    // 2. try to put bucket[i] into topK(heap) 
                    // 3. mark bucket[i] is visited
                    _scanner(bucket[i]); 
            }
        }

        /**
         * return the next bucket should be visited/probed.
         * should be implement by specific class, such as Hamming Ranking for BitIndex, Int Ranking for IntIndex...
         */
        virtual const vector<int>& NextBucket() = 0;

        virtual bool NextBucketExisted() const {
            return GetNumItemsProbed() < _para.base_size;
        }

        inline     int     GetNumItemsProbed() const {
            return _scanner.cnt();
        }

        inline const vector<pair<float, unsigned>> & GetSortedTopK() {
            return  _scanner.getMutableTopk().genTopk();
        }
    };
}
