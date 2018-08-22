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

#include <unordered_map>
#include <vector>
#include <random>
#include "map_index.hpp"

namespace ss {

    using namespace std;

    template<class DataType>
    class SRPIndex : public MapIndex<DataType, uint64_t > {

    public:
        explicit SRPIndex(const parameter& para) : MapIndex<DataType, uint64_t >(para) {}

        void Train(const lshbox::Matrix<DataType> &data) override {


            std::default_random_engine generator;
            std::normal_distribution<DataType > distribution(0.0, 1.0);

            for (int i=0; i<this->_para.num_bit; i++) {
                for (int j=0; j<this->_para.dim; j++) {
                    this->_projectors[i][j] = distribution(generator);
                }
            }
        }

    protected:

        uint64_t Quantize(const DataType *data) override {
            uint64_t value = 0;
            for (int i=0; i<this->_para.num_bit; i++) {
                DataType quantization = ss::InnerProduct(data,  this->_projectors[i].data(), this->_para.dim) ;
                value <<= 1;
                value |= (quantization>0) ? 1 :0;
            }
            return value;
        }

    };

}
