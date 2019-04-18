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
#include "rptree.hpp"
#include "joint_inverted.hpp"
#include "../index.hpp"

namespace ss {

    using std::vector;

    template<class DataType>
    class MultiPQIndex : public Index<DataType > {
    protected:
        vector<KMeansIndex<DataType > * >   _left_joint_index;
        vector<KMeansIndex<DataType > * >   _right_joint_index;
        int                                 _left_dimension;
        int                                 _right_dimension;
    public:
        explicit MultiPQIndex(const parameter& para) :
            Index<DataType >(para), _left_joint_index(para.forest_size), _right_joint_index(para.forest_size)  {
        }
        ~MultiPQIndex() {
            for (KMeansIndex<DataType> * pointer : _left_joint_index) { if (pointer) delete pointer;}
            for (KMeansIndex<DataType> * pointer : _right_joint_index) { if (pointer) delete pointer; }
        }

        void Train(const Matrix<DataType > & data) override {
            _left_dimension = data.getDim() / 2;
            _right_dimension = data.getDim() - data.getDim() / 2;
            auto left = ss::GroupCenters(
                    Visitor<DataType >(data, 0, _left_dimension), this->_para);
            auto right = ss::GroupCenters(
                    Visitor<DataType >(data, _left_dimension, _right_dimension), this->_para);

            for (int l = 0; l < this->_para.forest_size; ++l) {
                _left_joint_index[l] = new KMeansIndex<DataType >(this->_para);
                _left_joint_index[l]->set_centers(left[l]);

                _right_joint_index[l] = new KMeansIndex<DataType >(this->_para);
                _right_joint_index[l]->set_centers(right[l]);
            }
        }

        void Add(const Matrix<DataType > & data) override {

            for (int l = 0; l < this->_para.forest_size; ++l) {
                _left_joint_index[l]->Assign(Visitor<DataType >(data, 0, _left_dimension));
                _right_joint_index[l]->Assign(Visitor<DataType >(data, _left_dimension, _right_dimension));
            }
        }

        void Search(const DataType *query, const std::function<void (int)>& prober) override {
            for (int l = 0; l < this->_para.forest_size; ++l) {
                const vector<int>& left_idx = _left_joint_index[l]->Search(query);

                for (int i = 0; i < this->_para.forest_size; ++i) {
                    const vector<int>& right_idx = _right_joint_index[i]->Search(query + _left_dimension);
                    std::vector<int> indices;
                    indices.reserve(std::min(left_idx.size(), right_idx.size()));

                    std::set_intersection(
                            left_idx.begin(),left_idx.end(),
                            right_idx.begin(),right_idx.end(),
                            std::back_inserter(indices));
                    for (int id : indices) {
                        prober(id);
                    }
                }
            }
        }

    };

} // namespace ss
