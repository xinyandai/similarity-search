#pragma once

#include<bitset>
#include<functional>

#include "radixsorter.hpp"

#include "../parameters.hpp"
#include "../query.hpp"
#include "../index/bitindex.hpp"

namespace ss {

	using namespace lshbox;

	template <class DataType>
	class HRQuery : public Query<DataType > {

		using AccessorType 	= typename lshbox::Matrix<DataType>::Accessor;
		using KeyType 		= unsigned long long;

	protected:
		std::unordered_map<KeyType, vector<int >, ss::SSHasher<KeyType> > &	_index_map;
		ss::RadixSorter<DataType, KeyType> *					_sorter;

	public:
		~HRQuery() { if(_sorter) delete _sorter; }

		HRQuery(BitIndex<DataType > * 		index, 
			DataType * 			query, 
			lshbox::Metric<DataType > & 	metric, 
			const AccessorType & 		accessor, 
			parameter & 			para) 
			:  
			Query<DataType >(index, query, metric, accessor, para),
			_index_map(index->getIndexMap()) {	
			
			KeyType query_hash = index->hash_query(query);
			auto distor = [query_hash](const KeyType & index_key) {
				return  ss::countBitOne(query_hash ^ index_key) ;
			};
			_sorter = new RadixSorter<DataType, KeyType>(index->getIndexMap(), distor, para.num_bit);
		}


		virtual const 	vector<int>&	nextBucket() 	{ return _index_map[_sorter->nextBucket()]; }
		virtual bool	nextBucketExisted()		{ return _sorter->nextBucketExisted(); }
	
	};
}
