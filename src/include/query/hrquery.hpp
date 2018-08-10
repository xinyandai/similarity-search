#pragma once

#include "../parameters.hpp"
#include "../query.hpp"
#include "../bitindex.hpp"


namespace ss {

	using namespace lshbox;

	template <class DataType>
	class HRQuery : public Query<DataType > {
		using AccessorType = typename lshbox::Matrix<DataType>::Accessor;

	protected:
		vector<vector<unsigned long long > > _dist_bucket;
		unordered_map<unsigned long long, vector<int> > & _index_map;
		int _i;
		int _j;
	public:
		HRQuery(BitIndex<DataType >* index, 
			DataType* query, 
			lshbox::Metric<DataType >& metric, 
			const AccessorType& accessor, parameter& para) 
			:  
			Query<DataType >(index, query, metric, accessor, para),
		       	_dist_bucket(para.num_bit+1, vector<unsigned long long>()),
			_index_map(index->getIndexMap()),
			_i(0),
			_j(0) {
			
			unsigned long long query_hash = index->hash_query(query);
			for(auto it = _index_map.begin(); it != _index_map.end(); it++) {
				
				unsigned long long  index_hash = it->first;
				int hamming_dist = ss::countBitOne(query_hash ^ index_hash);	
				_dist_bucket[hamming_dist].push_back(index_hash);

				assert(it->second.size() > 0);
			}
		}


		vector<int>& nextBucket() {
			while(_j == _dist_bucket[_i].size()) {
				_j = 0;
				_i ++;
			}

			unsigned long long current_bucket = _dist_bucket[_i][_j++];
			assert(_index_map[current_bucket].size() > 0);

			return _index_map[current_bucket];
		}

		virtual bool 	nextBucketExisted() { return _i < _dist_bucket.size(); }
	
	};
}
