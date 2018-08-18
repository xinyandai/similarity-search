//////////////////////////////////////////////////////////////////////////////
/// Copyright 2018-present Xinyan DAI<xinyan.dai@outlook.com>
///
/// permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
/// documentation files (the "Software"), to deal in the Software without restriction, including without limitation
/// the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
/// and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in all copies or substantial portions of
/// the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
/// THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
/// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.

/// @version 0.1
/// @author  Xinyan DAI
/// @contact xinyan.dai@outlook.com
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cmath>
#include <iostream>

#include "vector"

namespace ss {

	using namespace std;

	template <class DataType >
	DataType inline InnerProduct(const DataType *a, const DataType *b, int dim) {

		DataType sum = .0;
		for (int i = 0; i < dim; ++i) {
			sum += a[i] * b[i];
		}
		return sum;
	}


	template <class DataType>
	DataType inline DiffProduct(const DataType *a, const DataType *means, const DataType *b, int dim) {
	
		DataType sum = .0;
		for (int i = 0; i < dim; ++i) {
			sum += (a[i] - means[i]) * b[i];
		}
		return sum;
	}

	template <class DataType>
	DataType inline CalculateNorm(const DataType *a, int dim) {
		return std::sqrt(InnerProduct(a, a, dim));
	}

	template <class DataType>
	void inline ScaleData(DataType *target, const DataType *data, DataType scale, int dim) {
		for (int d = 0; d<dim; d++) {
			target[d] = data[d] / scale;
		}
	}

	template <class DataType>
	void inline SimpleLSHTransform(DataType *transform_buffer, const DataType *data, DataType norm, DataType scale,
								   int origin_dim) {
		
			if (scale<norm) {
				std::cerr <<"the vector's scale: "<< scale <<" is smaller than norm: " << norm <<std:: endl; 
				scale = norm;
			}
		ss::ScaleData(transform_buffer, data, scale, origin_dim);
			transform_buffer[origin_dim] = std::sqrt(1 - norm*norm/scale/scale);
	}

	template <class DataType>
	void inline SimpleLSHTransform(DataType *transform_buffer, const DataType *data, int origin_dim) {
		DataType norm = ss::CalculateNorm(data, origin_dim);
		ss::ScaleData(transform_buffer, data, norm, origin_dim);
		transform_buffer[origin_dim] = 0.0;
	}

	int inline CountBitOne(uint64_t xorval) {
		int count = 0;

		while(xorval != 0) {
			count++;
			xorval &= (xorval-1);
		}
		return count;
	}

	template <class DataType >
	int inline CountDiff(const DataType *a, const DataType *b, int dim) {
		int count = 0;
		for (int d=0; d<dim; d++) {
			if (a[d]!=b[d]) {
				count++;
			}
		}
		return count;
	}

}
