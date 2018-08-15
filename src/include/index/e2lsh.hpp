#pragma once

#include <unordered_map>
#include <vector>
#include <random>
#include "int_index.hpp"
namespace ss {

	using namespace std;

	template<class DataType>
	class E2LSHIndex : public IntIndex<DataType> {
	public:	
		E2LSHIndex(parameter& para) : IntIndex<DataType >(para) {}

		virtual void train(const lshbox::Matrix<DataType > & ) {

			std::default_random_engine generator;
			std::normal_distribution<DataType > distribution(0.0, 1.0);
			
			for (int i=0; i<this->_para.num_bit; i++) {
				for (int j=0; j<this->_para.dim; j++) {
					this->_projectors[i][j] = distribution(generator);
				}
			}
			this->_r = this->_para.r;
		} 

	};
	
}
