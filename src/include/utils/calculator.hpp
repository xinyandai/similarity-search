#pragma once

#include <cmath>
#include "vector"

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
