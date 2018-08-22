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

#include <vector>
#include <random>
#include <unordered_map>
#include <boost/progress.hpp>

#include "kmeans.hpp"

#include "../utils/hashers.hpp"
#include "../index.hpp"

namespace ss {

    using namespace std;
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


        void Train(const lshbox::Matrix<DataType > & data) override {

            for (int i=0; i<_K_means.size(); ++i) {
                _K_means[i].iterate(lshbox::Visitor<DataType >(data, _offsets[i], _dimensions[i]));
            }
        }

        void Add(const lshbox::Matrix<DataType > & data) override {

            // find the nearest center
            for (int i = 0; i < data.getSize(); ++i) {
                
                vector<int > cluster_id(this->_para.num_codebook, 0);
                for (int code_book = 0; code_book < this->_para.num_codebook; ++code_book) {
                    cluster_id[code_book] = _K_means[code_book].NearestCenter(
                            data[i] + _offsets[code_book], _dimensions[code_book]);
                }
                
                _hash_map[cluster_id].push_back(i);
            }
        }

        
        HashTableType & hash_map() {
            return _hash_map;
        }


        vector<vector<std::pair<DataType, int>  > > DistToCenters(DataType* queryPoint) {

            vector<vector<std::pair<DataType, int>   > > dist_to_centers(
                    this->_para.num_codebook,
                    vector<std::pair<DataType, int>  >(this->_para.kmeans_centers)
            );

            for (int code_book = 0; code_book < this->_para.num_codebook ; ++code_book) {

                vector<pair<float, int> > & dists = dist_to_centers[code_book];
                for (int center = 0; center < this->_para.kmeans_centers; ++center) {

                    DataType distance = _K_means[code_book].Distance(
                            queryPoint + _offsets[code_book], _dimensions[code_book], center);
                    dists[center] = std::make_pair(distance, center);
                }
                std::sort(
                        dists.begin(),
                        dists.end(),
                        [](const pair<float, int>& a, const pair<float, int>& b) {
                            if (a.first != b.first)
                                return a.first < b.first;
                            else
                                return a.second < b.second;
                        });
            }

            return dist_to_centers;
        }


    };

}
