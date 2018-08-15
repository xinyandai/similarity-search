#pragma once

#include <vector>
#include <algorithm>
#include <cmath>

#include "../utils/calculator.hpp"

#include "bit_index.hpp"
#include "srp.hpp"

namespace ss {
	
	using namespace std;

	template<class DataType, class KeyType, class HashingIndexType>
	class SimpleLSHIndex : public MapIndex<DataType, KeyType > {
	protected:
		ss::MapIndex<DataType, KeyType > * 	_index;
		std::vector<DataType>			_norms;
		DataType 				_max_norm;
		vector<DataType >  			_tranformed_data; // TODO UNsafe when use multi thread
	public:
		~SimpleLSHIndex() { 
			delete _index; 
		}

		SimpleLSHIndex(parameter& para) : 
			MapIndex<DataType, KeyType >(para), 
			_index(new HashingIndexType(para)),
		       	_tranformed_data(para.dim + 1) {}

		virtual void train(const lshbox::Matrix<DataType >& data) {

			_norms = data.calNorms(); // TODO should be optimized
			_max_norm = *std::max_element(std::begin(_norms), std::end(_norms));
			
			_index->train(data);
		}


		virtual KeyType hash_data(const DataType * data, int id) {

			ss::simpleLSH_transform(_tranformed_data.data(), data, _norms[id], _max_norm, this->_para.origin_dim);
			return _index->hash_data(_tranformed_data.data(), id);
		}

		virtual KeyType hash_query(const DataType * data) {

			ss::simpleLSH_transform(_tranformed_data.data(), data, this->_para.origin_dim);
			return _index->hash_query(_tranformed_data.data());
		}

	protected:
		virtual KeyType quantize(const DataType * data)  {std::cerr << "should nerver be called" << std::endl;}
	};
}
