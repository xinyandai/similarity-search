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
		
		virtual void train(const lshbox::Matrix<DataType >& ) {} 
		virtual void add  (const lshbox::Matrix<DataType >& ) {}

		virtual ~Index() {}
	};
}

