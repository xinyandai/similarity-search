//////////////////////////////////////////////////////////////////////////////
/// Copyright 2018-present Xinyan DAI<xinyan.dai@outlook.com>
///
/// permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
/// documentation files (the "Software"), to deal in the Software without restriction, including without limitation
/// the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
/// and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in all copies or substantial portions of
/// the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
/// THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
/// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.

/// @version 0.1
/// @author  Xinyan DAI
/// @contact xinyan.dai@outlook.com
//////////////////////////////////////////////////////////////////////////////

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

	template<class DataType, class KeyType=uint64_t, class HashingIndexType=SRPIndex<DataType > >
	class NormRangeIndex : public BitIndex<DataType > {
	protected:
		BitIndex<DataType >* 	_index;
		const int 				_num_sub_data_set;
		const int  				_bit_sub_data_set;
		std::vector<DataType>	_norms;
		std::vector<DataType>	_percentiles;
		vector<DataType >  		_transformed_data; // TODO unsafe when use multi thread
	public:
		~NormRangeIndex() { 
			delete _index; 
		}

		explicit NormRangeIndex(parameter& para) :
				BitIndex<DataType >(para),
				_index(new HashingIndexType(para)),
				_transformed_data(para.dim + 1),
				_num_sub_data_set(para.num_sub_data_set),
				_bit_sub_data_set((int)std::ceil(std::log2(_num_sub_data_set))) {}

		void Train(const lshbox::Matrix<DataType> &data) override {
			_norms = data.calNorms(); //TODO should be optimized
			InitializePercentile();
            _index->Train(data);
		}


		KeyType HashData(const DataType *data, int id) override {
			int sub_data_set = GetSubDataSet(id);
			DataType scale = _percentiles[sub_data_set];

            ss::SimpleLSHTransform(_transformed_data.data(), data, _norms[id], scale, this->_para.origin_dim);
			KeyType hash_value = _index->HashData(_transformed_data.data(), id);
			return this->MergeBucketKey(hash_value, sub_data_set);
		}

		KeyType HashQuery(const DataType *data, int id) override {

            ss::SimpleLSHTransform(_transformed_data.data(), data, this->_para.origin_dim);
			return _index->HashQuery(_transformed_data.data(), id);
		}

		int GetSubDataSet(int id) { //TODO should be optimized
			assert(id>=0 && id<_norms.size());
			DataType norm = _norms[id];
			int i = 0;
			while(i < _percentiles.size() && _percentiles[i++] <  norm) {}
			// if i==_percentiles.size() , we use i-1; if _percentiles[i]<=norm we use i (we use 'i-1' because of 'i++')
			return i-1;
		}


		std::pair<int, DataType> HashDistAndPercentile(const KeyType &bucket_key, const KeyType &query_hash) {
			static KeyType mask = (1<<_bit_sub_data_set) - 1; //2^i - 1 == 000000111111(i's 1 in low bits)
			int sub_data_set = bucket_key & mask;
			int hash_dist = ss::CountBitOne(query_hash ^ (bucket_key >> _bit_sub_data_set));
			return std::make_pair(hash_dist, _percentiles[sub_data_set]);
		}
		KeyType MergeBucketKey(const KeyType &hash_val, int sub_data_set) {
			return  (hash_val << _bit_sub_data_set) | sub_data_set;
		}

	protected:
		void InitializePercentile() {
			std::vector<DataType > norms = _norms;
			std::sort(norms.begin(), norms.end());
			
			_percentiles.resize(_num_sub_data_set);
			for (int i=0; i<_num_sub_data_set; i++) {
				_percentiles[i] = norms[ norms.size() / _num_sub_data_set * (i+1) + norms.size() % _num_sub_data_set -1 ];
			}

		}

	};
}
