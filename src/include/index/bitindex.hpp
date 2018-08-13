#pragma once

#include <unordered_map>
#include <vector>

#include "../index.hpp"

namespace ss {

	using namespace std;

	template<class DataType>
	class BitIndex : public Index<DataType> {
	
	protected:
		/**
		 * if you want to make data 0-means in each dimension, then you should invoke #initilize_mean() in #train(); 
		 * if not, let it alone and means are initilized with default value 0.
		 */
		vector<DataType >				_means;
		unordered_map<unsigned long long, vector<int> > _hash_map;
		vector<vector<DataType > >			_projectors;
	public:	
		BitIndex(parameter& para) : 
			Index<DataType >(para), 
			_means(para.dim, 0),
			_projectors(para.num_bit, vector<DataType >(para.dim, 0)) {}

		unordered_map<unsigned long long, vector<int> > & getIndexMap() { return _hash_map;  }

		virtual void train(lshbox::Matrix<DataType >& ) = 0; 

		virtual void add  (lshbox::Matrix<DataType >& data) {
			
			for (int i=0; i<data.getSize(); ++i) {
				unsigned long long hash_val = hash_data(data[i], i);
				_hash_map[hash_val].push_back(i);
			}
		}

		virtual unsigned long long hash_data(DataType * data, int id=-1)	{ return this->quantize(data, 0); }
		virtual unsigned long long hash_query(DataType * query) 		{ return this->hash_data(query); }

	protected:

		virtual unsigned long long quantize(DataType * data, DataType quantizor) {
			unsigned long long value = 0;
			for (int i=0; i<this->_para.num_bit; i++) {
				DataType quantization =  ss::diff_product(data,  _means.data(), _projectors[i].data(), this->_para.dim ) ;	
				value <<= 1;
				value |= (quantization>quantizor) ? 1 :0;
			}

			return value;
		}

		void initilize_means(lshbox::Matrix<DataType >& train_data) {

			vector<long double> sum(this->_para.dim, .0);	// use long double to make sure presision
			for (int i=0; i < train_data.getSize(); i++) {	// one vector by one vector cache-friendly
				for( int d = 0; d < this->_para.dim; d++ ) {
					sum[d] += train_data[i][d] ;
				}
			}
			for (int d = 0; d < this->_para.dim; d++) {
				this->_means[d]  = sum[d] / train_data.getSize();
			}
		}

	};
	
}
