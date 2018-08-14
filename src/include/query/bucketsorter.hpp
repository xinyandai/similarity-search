#pragma once

#include <unordered_map>
#include <vector>
#include <functional>
#include <algorithm>

#include "../utils/hashers.hpp"

namespace ss {

	using namespace std;

	template<class DataType, class KeyType>
	class BucketSorter {
	protected:
		vector<pair<DataType, KeyType > > _sorted_bucket;
		int index = 0; 
	public:
		//TODO remove the hasher function
		BucketSorter(const unordered_map<KeyType, vector<int>, ss::SSHasher<KeyType > >& map, const std::function<DataType (const KeyType&)>& distor) {
			_sorted_bucket.reserve(map.size());
			for(auto it = map.begin(); it!=map.end(); it++) {
				DataType distance = distor(it->first);
				_sorted_bucket.emplace_back(std::make_pair(distance, it->first ) );
			}

			std::sort(
				_sorted_bucket.begin(), 
				_sorted_bucket.end(), 
               			[](const pair<float, unsigned>& a, const pair<float, unsigned>& b) {
					if (a.first != b.first)
						return a.first < b.first;
					else 
						return a.second < b.second;
				});
		}

		KeyType nextBucket() {
			return _sorted_bucket[index++].second;
		}

		bool nextBucketExisted() {
			return index < _sorted_bucket.size();
		}
	};
}
