#pragma once

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
			
			KeyType query_hash = index->hash_query(query);

			auto distor = [&](const KeyType & key) {
				int hamming_dist = ss::countBitOne(query_hash ^ key);	
				int num_same_bit = para.num_bit - hamming_dist;
				int interval_index = key & index->get_sub_data_set_mask();
				DataType dist = - num_same_bit * index->get_percentile(interval_index);
				return dist;
			};
			_bucket_sorter = new BucketSorter<DataType, KeyType>(_index_map, distor);
		}


		virtual vector<int>&	nextBucket() { return _index_map[_bucket_sorter->nextBucket()]; }
		virtual bool 		nextBucketExisted() { return _bucket_sorter->nextBucketExisted(); }
	
	};
}
