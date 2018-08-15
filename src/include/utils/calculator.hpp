#pragma once

#include <cmath>
#include "vector"

#include <iostream>

namespace ss {

	using namespace std;

	template <class DataType >
	DataType inline inner_product(const DataType * a, const DataType * b, int dim) {

		DataType sum = .0;
		for (int i = 0; i < dim; ++i) {
			sum += a[i] * b[i];
		}
		return sum;
	}


	template <class DataType>
	DataType inline diff_product(const DataType * a, const DataType * means, const DataType * b, int dim) {
	
		DataType sum = .0;
		for (int i = 0; i < dim; ++i) {
			sum += (a[i] - means[i]) * b[i];
		}
		return sum;
	}

	template <class DataType>
	DataType inline calculate_norm(const DataType * a, int dim) {
		return std::sqrt(inner_product(a, a, dim));
	}

	template <class DataType>
	void inline scale_data(DataType * target, const DataType * data, DataType scale, int dim) {
		for (int d = 0; d<dim; d++) {
			target[d] = data[d] / scale;
		}
	}

	template <class DataType>
	void inline simpleLSH_transform(DataType * transform_buffer, const DataType * data, DataType norm, DataType scale, int origin_dim) {
		
			if (scale<norm) {
				std::cerr <<"the vector's scale: "<< scale <<" is smaller than norm: " << norm <<std:: endl; 
				scale = norm;
			}
			ss::scale_data(transform_buffer, data, scale, origin_dim);
			transform_buffer[origin_dim] = std::sqrt(1 - norm*norm/scale/scale);
	}

	template <class DataType>
	void inline simpleLSH_transform(DataType * transform_buffer, const DataType * data, int origin_dim) {
		DataType norm = ss::calculate_norm(data, origin_dim);
		ss::scale_data(transform_buffer, data, norm, origin_dim);
		transform_buffer[origin_dim] = 0.0;
	}

	int inline countBitOne(unsigned long long xorval) {
		int count = 0;

		while(xorval != 0) {
			count++;
			xorval &= (xorval-1);
		}
		return count;
	}

	template <class DataType >
	int inline count_diff(const DataType * a,  const DataType * b, int dim) {
		int count = 0;
		for (int d=0; d<dim; d++) {
			if (a[d]!=b[d]) {
				count++;
			}
		}
		return count;
	}

}
