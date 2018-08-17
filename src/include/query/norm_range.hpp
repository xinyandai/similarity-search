#pragma once

#include <cmath>

#include "sorter/bucket_sorter.hpp"

#include "../parameters.hpp"
#include "../query.hpp"
#include "../index/norm_range.hpp"

namespace ss {

	using namespace lshbox;

	template <class DataType>
	class NormRangeQuery : public Query<DataType > {

		using AccessorType = typename lshbox::Matrix<DataType>::Accessor;
		using KeyType      = unsigned long long;

	protected:
		unordered_map<KeyType, vector<int>, ss::SSHasher<KeyType> > &	_index_map;
		BucketSorter<DataType, KeyType > *				_bucket_sorter;

	public:
		~NormRangeQuery() {delete _bucket_sorter; }

		NormRangeQuery( NormRangeIndex<DataType > * 	index, 
				DataType * 			query, 
				lshbox::Metric<DataType > & 	metric, 
				const AccessorType & 		accessor, 
				parameter & 			para) 
				:  
				Query<DataType >(index, query, metric, accessor, para),
				_index_map(index->getIndexMap()) {
			
			KeyType query_hash = index->hash_query(query, -1);
			const DataType PI = std::acos(-1);

			auto distor = [&query_hash, &para, index, PI](const KeyType & key) {
				std::pair<int, DataType > hash_dist_and_norm = index->hash_dist_and_percentile(key, query_hash);	
				int num_same_bit = para.num_bit - hash_dist_and_norm.first;
				DataType u = hash_dist_and_norm.second;
				DataType dist = - (num_same_bit * u * u  ) ;
				// DataType probability = num_same_bit / (DataType)para.num_bit;
				// DataType scaled_probability = std::min(1.0f, probability + 3 * probability * (1-probability));
				// DataType alpha = 0.3f;
				// DataType scaled_probability = std::min(1.0f, probability * (1-alpha) + alpha);
				// DataType dist = - u * std::cos( PI * (1.0f - scaled_probability) );
				return dist;
			};
			_bucket_sorter = new BucketSorter<DataType, KeyType>(_index_map, distor);
		}


		virtual vector<int>&	nextBucket() { return _index_map[_bucket_sorter->nextBucket()]; }
		virtual bool 		nextBucketExisted() { return _bucket_sorter->nextBucketExisted(); }
	
	};
}
