#pragma once

#include <unordered_map>
#include <vector>
#include "index.hpp"
#include "calculator.hpp"

namespace ss {

	using namespace std;

	template<class DataType>
	class BitIndex : public Index<DataType> {
	
	protected:
		unordered_map<unsigned long long, vector<int> > _hash_map;
		vector<vector<DataType > >			_projectors;
	public:	
		BitIndex(parameter& para) : 
			Index<DataType >(para), 
			_projectors(para.num_bit, vector<DataType >(para.dim, 0)) {}

	
		unordered_map<unsigned long long, vector<int> > & getIndexMap() { return _hash_map;  }

		virtual void train(lshbox::Matrix<DataType >) = 0; 

		virtual void add  (lshbox::Matrix<DataType > data) {
			
			for (int i=0; i<data.getSize(); ++i) {
				_hash_map[hash_data(data[i])].push_back(i);
			}
		}

		virtual unsigned long long hash_data(DataType * data) {
			
			unsigned long long value = 0;
			
			for (int i=0; i<this->_para.num_bit; i++) {

				value = value << 1;
				value |= ( ss::inner_product( _projectors[i].data(), data, this->_para.dim )  > 0) ? 1:0;	
			}

			return value;
		}

		virtual unsigned long long hash_query(DataType * query) { return this->hash_data(query); }

	};
	
}
