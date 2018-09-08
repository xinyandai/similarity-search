////////////////////////////////////////////////////////////////////////////////
/// Copyright 2018-present Xinyan DAI<xinyan.dai@outlook.com>
///
/// permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to
/// deal in the Software without restriction, including without limitation the
/// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
/// sell copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions ofthe Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
/// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
/// IN THE SOFTWARE.

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

    using std::vector;

    template<class DataType>
    class CrossPolytopeIndex : public MapIndex<DataType, vector<int > > {
    protected:
        vector<vector<vector<DataType > > > _project_matrix;
        int                                 _d_prime;
    public:
        explicit CrossPolytopeIndex(const parameter& para) :
                MapIndex<DataType, vector<int > >(para), _d_prime(4) {}

        void Train(const Matrix<DataType> &data) override {

            _project_matrix = vector<vector<vector<DataType> > >(
                    this->_para.num_bit, vector<vector<DataType> > (_d_prime, vector<DataType>(this->_para.dim)));

            std::default_random_engine generator;
            std::normal_distribution<DataType > distribution(0.0, 1.0);/// the way to generate

            for (int k=0; k<this->_para.num_bit; k++) {
                for (int d_prime = 0; d_prime < _d_prime; ++d_prime) {
                    for (int dim=0; dim<this->_para.dim; dim++) {
                        _project_matrix[k][d_prime][dim] = distribution(generator);
                    }
                }
            }
        }


        vector<vector<DataType > > Rotate(const DataType *data) const {
            vector<vector<DataType > > xs(this->_para.num_bit, vector<DataType >(_d_prime, 0.0f));
            for (int k = 0; k < this->_para.num_bit; ++k) {
                const vector<vector<DataType > > & A = _project_matrix[k];
                vector<DataType > & x = xs[k];
                ///x = Ax/||Ax||
                for (int d_prime = 0; d_prime < _d_prime; ++d_prime) {///x =  Ax
                    x[d_prime] = ss::InnerProduct(A[d_prime].data(), data, this->_para.dim);
                }
                ss::Normalize(x.data(), _d_prime);///x = ||x||
            }

            return xs;
        }

        vector<int > ArgMaxAbs(vector<vector<DataType > > & xs) const {
            vector<int > indexes(this->_para.num_bit, 0);
            for (int k = 0; k < this->_para.num_bit; ++k) {
                vector<DataType > & x = xs[k];
                long index = std::distance(
                        x.begin(),
                        std::max_element(
                                x.begin(),
                                x.end(),
                                [](const DataType& a, const DataType& b) {
                                    return std::abs(a) < std::abs(b);
                                })
                );
                indexes[k] = static_cast<int >(index);
            }
            return indexes;
        }

    protected:

        vector<int > Quantize(const DataType *data) override {
            vector<int > index_value(this->_para.num_bit, 1);
            vector<vector<DataType > > xs = Rotate(data);

            vector<int > indexes = ArgMaxAbs(xs);

            for (int k = 0; k < this->_para.num_bit; ++k) {
                vector<DataType > & x = xs[k];
                int index = indexes[k];
                index_value[k] = x[index] > 0 ? (index+1) : -(index+1); /// index of array start with 0
            }
            return index_value;
        }

    };

} // namespace ss
