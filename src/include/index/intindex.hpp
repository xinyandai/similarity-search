#pragma once

#include <unordered_map>
#include <vector>

#include "../index.hpp"
#include "../utils/hashers.hpp"

namespace ss {

	using namespace std;

	template<class DataType>
	class IntIndex : public Index<DataType> {
	
	protected:
		typedef vector<int> KeyType;
		unordered_map<KeyType, vector<int>, ss::SSHasher<KeyType> > _hash_map;
		vector<vector<DataType > >					_projectors;
		DataType							_r;
	public:	
		IntIndex(parameter& para) : 
			Index<DataType >(para), 
			_projectors(para.num_bit, vector<DataType >(para.dim, 0)) {}

		unordered_map<KeyType, vector<int>, SSHasher<KeyType > > & getIndexMap() { return _hash_map;  }

		virtual void train(const lshbox::Matrix<DataType >& ) = 0; 

		virtual void add  (const lshbox::Matrix<DataType >& data) {

			for (int i=0; i<data.getSize(); ++i) {
				KeyType hash_val = hash_data(data[i], i);
				_hash_map[hash_val].push_back(i);
			}
		}

		virtual KeyType hash_data (const DataType * data, int id)	{ return this->quantize(data); }
		virtual KeyType hash_query(const DataType * query) 		{ return this->hash_data(query, -1); }

	protected:

		virtual KeyType quantize(const DataType * data) {
			KeyType value(this->_para.dim, 0);
			for (int i=0; i<this->_para.num_bit; i++) {
				DataType quantization =  ss::inner_product(data, _projectors[i].data(), this->_para.dim ) ;	
				value[i] = std::ceil(quantization / _r);
			}
			return value;
		}

	};
	
}
