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

#include <utility>
#include <vector>
#include <random>
#include <unordered_map>
#include <boost/progress.hpp>

#include "kmeans.hpp"

#include "../utils/hashers.hpp"
#include "../index.hpp"

namespace ss {

    using std::vector;
    using std::unordered_map;
    using HashTableType = unordered_map< vector<int >, vector<int>, ss::SSHasher< vector<int > > >;

    template<class DataType>
    class PQIndex : public Index<DataType> {
        
    protected:
        vector<KMeansIndex<DataType > >  _K_means;
        vector<int >                     _dimensions;
        vector<int >                     _offsets;
        HashTableType                    _hash_map;
        
    public:
        explicit PQIndex(const parameter& para) :
                Index<DataType >(para),
                _K_means(para.num_codebook, KMeansIndex<DataType>(para)), _offsets(para.num_codebook) {

            int sub_dimension = this->_para.dim / this->_para.num_codebook;
            int reminder = this->_para.dim % this->_para.num_codebook;
            _dimensions.insert(_dimensions.end(), reminder, sub_dimension+1);
            _dimensions.insert(_dimensions.end(), this->_para.num_codebook - reminder, sub_dimension);

            _offsets[0] = 0;
            for (int i = 1; i < this->_para.num_codebook; ++i) {
                _offsets[i] = _offsets[i-1] + _dimensions[i-1];
            }
        }


        void Train(const Matrix<DataType > & data) override {

            for (int i=0; i<_K_means.size(); ++i) {
                /// train each code-book,
                /// use Visitor to represent data in each code-book
                _K_means[i].iterate(Visitor<DataType >(data, _offsets[i], _dimensions[i]));
            }
        }

        void Add(const Matrix<DataType > & data) override {

            for (int i = 0; i < data.getSize(); ++i) {
                /// find each point's nearest center
                vector<int > cluster_id(this->_para.num_codebook, 0);
                /// find each point's nearest center in each code-book
                for (int code_book = 0; code_book < this->_para.num_codebook; ++code_book) {
                    cluster_id[code_book] =
                            _K_means[code_book].NearestCenter(data[i] + _offsets[code_book], _dimensions[code_book]);
                }
                /// add into centers
                _hash_map[cluster_id].push_back(i);
            }
        }

        
        HashTableType & hash_map() {
            return _hash_map;
        }


        /**
         * dist_to_centers[i][j] means the distance between query and j'th center in i'th code-book.
         * @return dist_to_centers
         */
        vector<vector<std::pair<DataType, int> > > DistToCenters(const DataType* query) {
            vector<vector<std::pair<DataType, int>   > > dist_to_centers(this->_para.num_codebook);
            for (int code_book = 0; code_book < this->_para.num_codebook ; ++code_book) {
                dist_to_centers[code_book] =
                        _K_means[code_book].ClusterDistance(query + _offsets[code_book], _dimensions[code_book]);
            }

            return dist_to_centers;
        }

    };

} // namespace ss
