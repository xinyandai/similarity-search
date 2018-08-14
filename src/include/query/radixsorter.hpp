#pragma once

#include<bitset>

#include "../parameters.hpp"
#include "../query.hpp"
#include "../index/bitindex.hpp"
#include "../utils/hashers.hpp"

namespace ss {

	using namespace lshbox;

	template <class DataType, class KeyType >
	class RadixSorter {
		using AccessorType = typename lshbox::Matrix<DataType>::Accessor;

	protected:
		vector<vector<KeyType > >					 _dist_bucket;
		int _i;
		int _j;
	public:
		RadixSorter(
			unordered_map<KeyType, vector<int>, ss::SSHasher<KeyType > > & 	map, 
			const std::function<int (const KeyType&) > &			distor, 
			int 	maximum_distance) 
			: 
		       	_dist_bucket(maximum_distance+1, vector<KeyType >()),
			_i(0),
			_j(0) {
			
			for(auto it = map.begin(); it != map.end(); it++) {
			
				int hamming_dist = distor(it->first);
				assert(hamming_dist>=0 && hamming_dist<=maximum_distance);
				
				_dist_bucket[hamming_dist].push_back(it->first);
				assert(it->second.size() > 0);
			}
		}


		virtual KeyType& nextBucket() {
			while(_j == _dist_bucket[_i].size()) {
				_j = 0;
				_i ++;
			}
			return _dist_bucket[_i][_j++];
		}

		virtual bool 	nextBucketExisted() { return _i < _dist_bucket.size(); }
	
	};
}
