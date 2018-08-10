#pragma once

#include "matrix.h"
#include "parameters.hpp"

#include "utils/calculator.hpp"

namespace ss {


	template <class DataType>
	class Index {
	
	protected:
		parameter& _para;
	public:
		Index(parameter& para): _para(para) {}

		virtual void preprocess_train(lshbox::Matrix<DataType >& ) {}
		virtual void preprocess_base (lshbox::Matrix<DataType >& ) {}
		virtual void preprocess_query(lshbox::Matrix<DataType >& ) {}
		
		virtual void train(lshbox::Matrix<DataType >& ) {} 
		virtual void add  (lshbox::Matrix<DataType >& ) {}

		virtual ~Index() {}
	};
}

