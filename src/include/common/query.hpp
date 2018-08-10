#pragma once

#include "parameters.hpp"
#include "index.hpp"

#include "../lshbox/topk.h"
#include "../lshbox/metric.h"

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
			
		virtual void 	probeItems(int numItems) {
			while(getNumItemsProbed() < numItems && nextBucketExisted()) {
				probeNextBucket();
			}
		}

		virtual bool 	nextBucketExisted() { return getNumItemsProbed() < _para.base_size; }

		virtual void 	probeNextBucket() {

			vector<int>& bucket = nextBucket();
			assert(bucket.size() > 0);

			for(int i=0; i<bucket.size(); i++)
				_scanner(bucket[i]);
		
		}
		
		virtual vector<int>& nextBucket() = 0;

		inline 	int 	getNumItemsProbed() const { return _scanner.cnt(); }
	
		inline 	const 	vector<pair<float, unsigned>>& getSortedTopK() { return  _scanner.getMutableTopk().genTopk(); }
	};
}
