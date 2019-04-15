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

#include "rptree.hpp"
#include "kmeans.hpp"
#include "../index.hpp"

namespace ss {

    using std::vector;

    template<class DataType>
    class JointInvertedIndex : public Index<DataType > {
    protected:
        KMeansIndex<DataType >              _kmeansIndex;
        vector<KMeansIndex<DataType > * >   _joint_index;
    public:
        explicit JointInvertedIndex(const parameter& para) :
                Index<DataType >(para), _kmeansIndex(para), _joint_index(para.forest_size) {
        }

        ~JointInvertedIndex() {
            for (KMeansIndex<DataType> * pointer : _joint_index) {
                if (pointer)
                    delete pointer;
            }
        }

        void Train(const Matrix<DataType > & data) override {
            int Ks = this->_para.kmeans_centers;
            int L = this->_para.forest_size;
            _kmeansIndex.reset(Ks * L);
            _kmeansIndex.Train(data);
            const vector<vector<DataType > > & list_centers= _kmeansIndex.get_centers();
            Matrix<DataType > centers(list_centers);

            parameter rpt_parameters = this->_para;
            { // depth of rpt should be log2(K)
                rpt_parameters.num_bit = 0;
                int temp_Ks = Ks;
                while (temp_Ks >>= 1)
                    ++rpt_parameters.num_bit;
            }
            RPTIndex<DataType > rpt(rpt_parameters);
            rpt.Train(centers);
            rpt.Add(centers);
            // number of leaf node: Ks
            // size of leaf: L
            vector<vector<int > * > leafIdx = rpt.LeafIdx();

            for (int l = 0; l < L; ++l) {
                _joint_index[l] = new KMeansIndex<DataType >(this->_para);
                vector<vector<DataType > > l_centers(Ks);
                for (int k = 0; k < Ks; ++k) {
                    l_centers[k] = list_centers[ (*(leafIdx[k]))[l] ];
                }
                _joint_index[l]->set_centers(l_centers);
            }
        }

        void Add(const Matrix<DataType > & data) override {
            for (int l = 0; l < this->_para.forest_size; ++l) {
                _joint_index[l]->Add(data);
            }
        }

        void Search(const DataType *query, const std::function<void (int)>& prober) override {
            for (int l = 0; l < this->_para.forest_size; ++l) {
                _joint_index[l]->Search(query, prober);
            }
        }

    };

} // namespace ss
