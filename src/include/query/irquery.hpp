#pragma once

#include "radixsorter.hpp"

#include "../parameters.hpp"
#include "../query.hpp"
#include "../index/intindex.hpp"
#include "../utils/calculator.hpp"

namespace ss {

	using namespace lshbox;

	template <class DataType>
	class IRQuery : public Query<DataType > {
		using AccessorType = typename lshbox::Matrix<DataType>::Accessor;
		using KeyType = vector<int >;
	protected:
		unordered_map<KeyType, vector<int>, ss::SSHasher<KeyType > > &	_index_map;
		ss::RadixSorter<DataType, KeyType> *				_sorter;
	public:
		~IRQuery() { delete _sorter; }
		IRQuery(IntIndex<DataType > *	 	index, 
			DataType * 			query, 
			lshbox::Metric<DataType > & 	metric, 
			const AccessorType & 		accessor, 
			parameter & 			para) 
			:  
			Query<DataType >(index, query, metric, accessor, para),
			_index_map(index->getIndexMap()) {
			
			KeyType query_hash = index->hash_query(query);
			auto distor = [query_hash] (const KeyType & index_key) {
				return ss::count_diff(query_hash.data(), index_key.data(), query_hash.size());
			};
			_sorter = new RadixSorter<DataType, KeyType>(index->getIndexMap(), distor, para.num_bit);
		}

		virtual const 	vector<int >& nextBucket() { return _index_map[_sorter->nextBucket()]; }
		virtual bool 	nextBucketExisted() { return _sorter->nextBucketExisted(); }
	
	};
}
