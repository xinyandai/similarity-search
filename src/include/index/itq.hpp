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

#include "map_index.hpp"

namespace ss {

    template<typename DataType>
    class ITQIndex: public MapIndex<DataType, uint64_t > {

        using KeyType = uint64_t;

    public:

        explicit ITQIndex(const parameter & para):  MapIndex<DataType, uint64_t >(para) {}

        ~ITQIndex() {}

        void Train(const Matrix<DataType> & data) override;

    protected:
        KeyType Quantize(const DataType *data) override  {
            KeyType hash_value = 0;
            std::vector<DataType > v(_eigen_vectors.size());
            for (unsigned i = 0; i != v.size(); ++i) {
                v[i] = ss::DiffProduct(data, this->_means.data(), _eigen_vectors[i].data(), _eigen_vectors[i].size());
            }
            for (unsigned i = 0; i != v.size(); ++i) {
                DataType  product  = ss::InnerProduct(v.data(), _rotate_matrix[i].data(), _rotate_matrix[i].size());
                hash_value <<= 1;
                hash_value |= product > 0? 1 : 0;
            }
            return hash_value;
        }

    private:

        std::vector<std::vector<DataType > >  _eigen_vectors;
        std::vector<std::vector<DataType> >   _rotate_matrix;

    };
} // namespace ss

// ------------------------- implementation -------------------------

template<typename DataType>
void ss::ITQIndex<DataType>::Train(const Matrix<DataType> & data) {

    this->InitializeMeans(data); /// TODO(Xinyan): should avoid re-computing means

    std::mt19937 rng(unsigned(std::time(0)));
    std::normal_distribution<DataType > nd;
    std::uniform_int_distribution<unsigned> usBits(0, data.getSize() - 1);

    {
        /// 1. wrap data with eigen
        Eigen::MatrixXf matrix_data(data.getSize(), data.getDim());
        for (int i = 0; i < data.getSize(); ++i) { /// TODO(Xinyan): to be improved
            std::vector<DataType > vals(data.getDim());
            for (int dimension = 0; dimension != data.getDim(); ++dimension) {
                vals[dimension] = data[i][dimension];
            }
            matrix_data.row(i) = Eigen::Map<Eigen::VectorXf>(&vals[0], data.getDim());
        }
        /// 2. zero-centered
        Eigen::MatrixXf centered = matrix_data.rowwise() - matrix_data.colwise().mean();
        /// 3. use eigen-vectors to project data
        Eigen::MatrixXf cov = (centered.transpose() * centered) / DataType (matrix_data.rows() - 1);
        Eigen::SelfAdjointEigenSolver<Eigen::MatrixXf> eig(cov);
        Eigen::MatrixXf eigen_vectors = eig.eigenvectors().rightCols(this->_para.num_bit);
        Eigen::MatrixXf V = matrix_data * eigen_vectors;
        // 4. initialize R
        Eigen::MatrixXf R(this->_para.num_bit, this->_para.num_bit);
        Eigen::JacobiSVD<Eigen::MatrixXf> svd(R, Eigen::ComputeThinU | Eigen::ComputeThinV);
        R = svd.matrixU();

        boost::progress_display progress(this->_para.iteration);
        for (int iter = 0; iter < this->_para.iteration; ++iter && ++progress) {

            Eigen::MatrixXf VR = V * R;
            Eigen::MatrixXf B(VR.rows(), VR.cols()); // n * c
            assert(VR.rows() == this->_para.train_size);
            assert(VR.cols() == this->_para.num_bit);

            for (unsigned i = 0; i != VR.rows(); ++i) {
                for (unsigned j = 0; j != VR.cols(); ++j) {
                    B(i, j) = VR(i, j) > 0 ? 1 : -1;
                }
            }
            Eigen::JacobiSVD<Eigen::MatrixXf> svd_tmp(B.transpose() * V, Eigen::ComputeThinU | Eigen::ComputeThinV);
            R = svd_tmp.matrixV() * svd_tmp.matrixU().transpose();
        }

        _rotate_matrix.resize(this->_para.num_bit);
        for (unsigned i = 0; i != _rotate_matrix.size(); ++i) {

            _rotate_matrix[i].resize(this->_para.num_bit);
            for (unsigned j = 0; j != _rotate_matrix[i].size(); ++j) {
                _rotate_matrix[i][j] = R(j, i);
            }
        }
        _eigen_vectors.resize(this->_para.num_bit);
        for (unsigned i = 0; i != _eigen_vectors.size(); ++i) {

            _eigen_vectors[i].resize(data.getDim());
            for (unsigned dimension = 0; dimension != data.getDim(); ++dimension) {
                _eigen_vectors[i][dimension] = eigen_vectors(dimension, i);
            }
        }
    }
}

