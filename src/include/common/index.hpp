#pragma once

#include "../lshbox/matrix.h"
#include "parameters.hpp"

namespace ss {


	template <class DataType>
	class Index {
		parameter& para_;
	public:
		Index(parameter& para): para_(para) {}

		virtual void preprocess_train(lshbox::Matrix<DataType > ) {}
		virtual void preprocess_base (lshbox::Matrix<DataType > ) {}
		virtual void preprocess_query(lshbox::Matrix<DataType > ) {}
		
		virtual void train(lshbox::Matrix<DataType >) {} 
		virtual void add  (lshbox::Matrix<DataType >) {}

		virtual ~Index() {}
	};
}

