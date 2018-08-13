#pragma once

#include <cmath>

namespace ss {

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

	int inline countBitOne(unsigned long long xorval) {
		int count = 0;

		while(xorval != 0) {
			count++;
			xorval &= (xorval-1);
		}
		return count;
	}

}
