#pragma once

#include <unordered_map>
#include <vector>

#include "map_index.hpp"

#include "../utils/hashers.hpp"

namespace ss {

	using namespace std;

	template<class DataType>
	class BitIndex : public MapIndex<DataType, unsigned long long > {

		using KeyType = unsigned long long;
	protected:
		DataType quantizor = 0;
	public:	
		BitIndex(parameter& para) : MapIndex<DataType, KeyType >(para){}

		virtual void train(const lshbox::Matrix<DataType >& ) = 0;

	protected:

		virtual KeyType quantize(const DataType * data) {
			KeyType value = 0;
			for (int i=0; i<this->_para.num_bit; i++) {
				DataType quantization =  ss::diff_product(data, this-> _means.data(), this->_projectors[i].data(), this->_para.dim ) ;	
				value <<= 1;
				value |= (quantization>quantizor) ? 1 :0;
			}
			return value;
		}
	};
	
}
