#pragma once

namespace ss {

	template <class DataType >
	DataType inner_product(const DataType * a, const DataType * b, int dim) {

		DataType sum = .0;
		for (int i = 0; i < dim; ++i) {
			sum += a[i] * b[i];
		}
		return sum;
	}


	int countBitOne(unsigned long long xorval) {
		int count = 0;

		while(xorval != 0) {
			count++;
			xorval &= (xorval-1);
		}
		return count;
	}
}
