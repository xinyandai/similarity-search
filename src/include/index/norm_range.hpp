#pragma once

#include <vector>
#include <algorithm>
#include <cmath>
#include <bitset>

#include "../utils/calculator.hpp"

#include "bit_index.hpp"
#include "srp.hpp"

namespace ss {
	
	using namespace std;

	template<class DataType, class KeyType=unsigned long long, class HashingIndexType=SRPIndex<DataType > >
	class NormRangeIndex : public BitIndex<DataType > {
	protected:
		BitIndex<DataType >* 	_index;
		int 			_num_sub_data_set;
		int 			_bit_sub_data_set;
		std::vector<DataType>	_norms;
		std::vector<DataType>	_percentiles;
		vector<DataType >  	_tranformed_data; // TODO UNsafe when use multi thread
	public:
		~NormRangeIndex() { 
			delete _index; 
		}

		NormRangeIndex(parameter& para) : BitIndex<DataType >(para), _index(new HashingIndexType(para)), _tranformed_data(para.dim + 1) {
			_num_sub_data_set = para.num_sub_data_set;
			_bit_sub_data_set = std::ceil(std::log2(_num_sub_data_set));
		}

		virtual void train(const lshbox::Matrix<DataType >& data) {
			_norms = data.calNorms(); //TODO should be optimized
			initialize_percentile();
			_index->train(data);
		}


		virtual KeyType hash_data(const DataType * data, int id) {
			int sub_data_set = get_sub_data_set(id);
			DataType scale = _percentiles[sub_data_set];

			ss::simpleLSH_transform(_tranformed_data.data(), data, _norms[id], scale, this->_para.origin_dim);
			KeyType hash_value = _index->hash_data(_tranformed_data.data(), id);
			return this->merge_bucket_key(hash_value, sub_data_set);
		}

		virtual KeyType hash_query(const DataType * data, int id) {

			ss::simpleLSH_transform(_tranformed_data.data(), data, this->_para.origin_dim);
			return _index->hash_query(_tranformed_data.data(), id);
		}

		int get_sub_data_set(int id) { //TODO should be optimized
			assert(id>=0 && id<_norms.size());
			DataType norm = _norms[id];
			int i = 0;
			while(i < _percentiles.size() && _percentiles[i++] <  norm) {}
			return i-1; // if i==_percentiles.size() , we use i-1; if _percentiles[i]<=norm we use i (we use 'i-1' because of 'i++')
		}


		virtual std::pair<int, DataType> hash_dist_and_percentile(const KeyType& bucket_key, const KeyType& query_hash) {
			static KeyType mask = (1<<_bit_sub_data_set) - 1; //2^i - 1 == 000000111111(i's 1 in low bits)
			int sub_data_set = bucket_key & mask;
			int hash_dist = ss::countBitOne(query_hash ^ (bucket_key >> _bit_sub_data_set));
			return std::make_pair(hash_dist, _percentiles[sub_data_set]);
		}
		virtual KeyType merge_bucket_key(const KeyType& hash_val, int sub_data_set) {
			return  (hash_val << _bit_sub_data_set) | sub_data_set;
		}

	protected:
		void initialize_percentile() {
			std::vector<DataType > norms = _norms;
			std::sort(norms.begin(), norms.end());
			
			_percentiles.resize(_num_sub_data_set);
			for (int i=0; i<_num_sub_data_set; i++) {
				_percentiles[i] = norms[ norms.size() / _num_sub_data_set * (i+1) + norms.size() % _num_sub_data_set -1 ];
			}

		}

	};
}
