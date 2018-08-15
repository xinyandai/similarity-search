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
		Index<DataType >*		_index;
		DataType*			_query;
		lshbox::Scanner<AccessorType >	_scanner;
		parameter& 			_para;

	public:
		Query(Index<DataType >* index, 
				DataType*  query, 
				lshbox::Metric<DataType >& metric, 
				const AccessorType& accessor, 
				parameter& para) 
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
		virtual void 	probeItems(int numItems) {

			while(getNumItemsProbed() < numItems && nextBucketExisted()) {

				const vector<int>& bucket = nextBucket();
				assert(bucket.size() > 0);

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
		virtual const 	vector<int>& nextBucket() = 0;

		virtual bool 	nextBucketExisted() const { return getNumItemsProbed() < _para.base_size; }
		inline 	int 	getNumItemsProbed() const { return _scanner.cnt(); }
		inline 	const 	vector<pair<float, unsigned>>& getSortedTopK() { return  _scanner.getMutableTopk().genTopk(); }
	};
}
