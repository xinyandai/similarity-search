#pragma once

#include "matrix.h"
#include "parameters.hpp"

#include "utils/calculator.hpp"

namespace ss {

	/**
	 * A interface of index structure which hold all base data.
	 * Specific Search method is provide in "include/query/" and they implement the interface of "include/query.hpp"
	 */
	template <class DataType>
	class Index {
	
	protected:
		parameter& _para;
	public:
		Index(parameter& para): _para(para) {}
	
		/**
		 * For Locality Sensitive Hashing, random vectors will be genrated for projection and the training data won't be used
		 * For Learning to Hashing, hashing functions will be learned
		 * For Graph Method, nothing will happen.
		 */	
		virtual void train(const lshbox::Matrix<DataType >& ) {} 

		/**
		 * all base data will be stored in this index strcture.
		 */
		virtual void add  (const lshbox::Matrix<DataType >& ) {}

		virtual ~Index() {}
	};
}

