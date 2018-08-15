#pragma once

#include <unordered_map>
#include <vector>

#include "map_index.hpp"
#include "../utils/hashers.hpp"

namespace ss {

	using namespace std;

	template<class DataType>
	class IntIndex : public MapIndex<DataType, vector<int > > {
		
		typedef vector<int> KeyType;
	
	protected:
		DataType _r;
	public:	
		IntIndex(parameter& para) : MapIndex<DataType, KeyType >(para), _r(para.r){}
		
		virtual void train(const lshbox::Matrix<DataType >& ) = 0; 

	protected:

		virtual KeyType quantize(const DataType * data) {
			KeyType value(this->_para.dim, 0);
			for (int i=0; i<this->_para.num_bit; i++) {
				DataType quantization =  ss::inner_product(data, this->_projectors[i].data(), this->_para.dim ) ;	
				value[i] = std::ceil(quantization / _r);
			}
			return value;
		}

	};
	
}
