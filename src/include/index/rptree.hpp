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

#include <eigen3/Eigen/Dense>

#include <map>
#include <vector>
#include <random>
#include <iostream>
#include <functional>
#include <boost/progress.hpp>

#include "index.hpp"

namespace ss {
    template<typename DataType>
    class Node {
    protected:
        DataType                            _medians;
        vector<DataType >                   _projector;
        vector<int >                        _idx;
        Node                                *_left;
        Node                                *_right;
        const u_int                         _depth;
        const u_int                         _dim;
        const u_int                         _max_depth;

    public:
        explicit Node(u_int dim, u_int depth, u_int max_depth) :
            _depth(depth), _max_depth(max_depth), _dim(dim), _projector(dim) {
        }

        bool IsLeaf() {
            return this->_depth + 1 == this->_max_depth;
        }

        void _GenerateProjector(const Matrix<DataType> &data, const vector<int> &idx) {
            // TODO(Xinyan) choose the eigenvector as random projection vector
            std::default_random_engine generator;
            std::normal_distribution<DataType > distribution(0.0, 1.0);

            for (int j=0; j<_dim; j++) {
                this->_projector[j] = distribution(generator);
            }
        }

        void MakeTree(const Matrix<DataType> &data, vector<int > idx) {
            if (IsLeaf()) {
                _idx = idx;
                return;
            }
            int N = idx.size();
            this->_GenerateProjector(data, idx);
            vector <DataType> projected_value(idx.size());
            for (int i = 0; i < idx.size(); ++i) {
                projected_value[i] = ss::InnerProduct(data[idx[i]], _projector.data(), _projector.size());
            }

            vector<size_t > sorted_idx = ss::SortIndexes(projected_value);
            vector<size_t > sorted_left_inx = vector<size_t>(sorted_idx.begin(), sorted_idx.begin() + N/2);
            vector<size_t > sorted_right_inx = vector<size_t >(sorted_idx.begin() + N/2, sorted_idx.end());

            this->_medians = (projected_value[sorted_idx[N/2-1]]+ projected_value[sorted_idx[N/2+1]]) / 2.0;

            this->_left = new Node(_dim, _depth + 1, _max_depth);
            this->_right = new Node(_dim, _depth + 1, _max_depth);
            this->_left->MakeTree(data, ss::FancyIndex(idx, sorted_left_inx));
            this->_right->MakeTree(data, ss::FancyIndex(idx, sorted_right_inx));
        }

        const vector<int>& ProbeLeaf(const DataType * query) {
            if (IsLeaf()) {
                return this->_idx;
            }
            DataType projected_value= ss::InnerProduct(query, _projector.data(), _projector.size());
            if (projected_value < _medians) {
                return this->_left->ProbeLeaf(query);
            } else {
                return this->_right->ProbeLeaf(query);
            }
        }

    };

    template<typename DataType>
    class RPTIndex: public Index<DataType > {
    protected:
        Node<DataType >             _root;
    public:
        explicit RPTIndex(const parameter & para) :
                Index <DataType >(para),
                _root(para.dim, 0, para.num_bit) {
        }

        ~RPTIndex() {}

        void Train(const Matrix<DataType> & data) override {
            this->_root.MakeTree(data, ss::Range<int>(0, data.getSize()));
        }

        void Add(const Matrix<DataType> &data) override {
        }

        void Search(const DataType* query, const std::function<void (int)>& prober) override {
            const vector<int >& idx = this->_root.ProbeLeaf(query);
            for (int id : idx) {
                prober(id);
            }
        }
    };
} // namespace ss


// ------------------------- implementation -------------------------

