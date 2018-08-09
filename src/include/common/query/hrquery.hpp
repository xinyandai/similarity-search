#pragma once

#include "../parameters.hpp"
#include "../query.hpp"
#include "../calculator.hpp"
#include "../bitindex.hpp"

#include "../../lshbox/topk.h"

namespace ss {

	using namespace lshbox;

	template <class DataType>
	class HRQuery : public Query<DataType > {
		using AccessorType = typename lshbox::Matrix<DataType>::Accessor;

	protected:
		vector<vector<vector<int>* > > _dist_bucket;
		int _i;
		int _j;
	public:
		HRQuery(BitIndex<DataType >* index, 
			DataType* query, 
			lshbox::Metric<DataType >& metric, 
			const AccessorType& accessor, parameter& para) 
			:  
			Query<DataType >(index, query, metric, accessor, para),
		       	_dist_bucket(para.num_bit+1, vector<vector<int>* >()),
			_i(0),
			_j(0) {
		
			unordered_map<unsigned long long, vector<int> > _index_map = index->getIndexMap();
			
			unsigned long long query_hash = index->hash_query(query);
			for(auto it = _index_map.begin(); it != _index_map.end(); it++) {
				
				unsigned long long  index_hash = it->first;
				int hamming_dist = ss::countBitOne(query_hash ^ index_hash);	
				_dist_bucket[hamming_dist].push_back(&(it->second));
			}
		}


		vector<int>& nextBucket() {
			vector<int >* current_bucket = _dist_bucket[_i][_j++];
			if (_j == _dist_bucket[_i].size()) {
				_j = 0;
				_i ++;
			} else {
				_j ++;
			}
			return *current_bucket;
		}	
	
	};
}
