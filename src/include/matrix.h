//////////////////////////////////////////////////////////////////////////////
/// Copyright (C) 2014 Gefu Tang <tanggefu@gmail.com>. All Rights Reserved.
///
/// This file is part of LSHBOX.
///
/// LSHBOX is free software: you can redistribute it and/or modify it under
/// the terms of the GNU General Public License as published by the Free
/// Software Foundation, either version 3 of the License, or(at your option)
/// any later version.
///
/// LSHBOX is distributed in the hope that it will be useful, but WITHOUT
/// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
/// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
/// more details.
///
/// You should have received a copy of the GNU General Public License along
/// with LSHBOX. If not, see <http://www.gnu.org/licenses/>.
///
/// @version 0.1
/// @author Gefu Tang & Zhifeng Xiao
/// @date 2014.6.30
//////////////////////////////////////////////////////////////////////////////

/**
 * @file matrix.h
 *
 * @brief Dataset management class.
 */
#pragma once
#include <fstream>
#include <vector>
#include <cmath>
#include <iostream>
#include <cstring>

#include <assert.h>
#include "utils/calculator.hpp"

namespace lshbox {

    template <class T>
    class Matrix {

        int _dimension;
        int _size;
        T * _data;

    public:

        ~Matrix() {
            std::free(_data);
        }
        explicit Matrix(const std::string &path): _data(NULL) {
            loadFvecs((*this), path);
        }
        Matrix(const Matrix& M) = delete;
        Matrix& operator=(const Matrix& M)  = delete;

        void reset(int _dim, int _N) {

            _dimension = _dim;
            _size = _N;
            _data = (T*)std::realloc(_data, _dimension * _size * sizeof(T));
        }

        const T *operator [] (int i) const {
            return _data + i * _dimension;
        }

        T *operator [] (int i) {
            return _data + i * _dimension;
        }

        int getDim() const {
            return _dimension;
        }

        int getSize() const {
            return _size;
        }

        std::vector<T> calNorms() const {
            std::vector<T> norms(_size);
            for (int i = 0; i < _size; ++i) {
                norms[i] = ss::CalculateNorm((*this)[i], _dimension);
            }
            return norms;
        }

        template<typename DATATYPE>
        friend void loadFvecs(Matrix<DATATYPE>& data, const std::string& dataFile) {
            std::ifstream fin(dataFile.c_str(), std::ios::binary | std::ios::ate);
            if (!fin) {
                std::cout << "cannot open file " << dataFile.c_str() << std::endl;
                assert(false);
            }
            uint64_t fileSize = fin.tellg();
            fin.seekg(0, fin.beg);
            assert(fileSize != 0);

            int dimension;
            fin.read((char*)&dimension, sizeof(int));
            unsigned bytesPerRecord = dimension * sizeof(DATATYPE) + 4;
            assert(fileSize % bytesPerRecord == 0);
            uint64_t cardinality = fileSize / bytesPerRecord;

            data.reset(dimension, cardinality);
            fin.read((char *)(data[0]), sizeof(float) * dimension);

            int dim;
            for (int i = 1; i < cardinality; ++i) {
                fin.read((char*)&dim, sizeof(int));
                assert(dim == dimension);
                fin.read((char *)(data[i]), sizeof(float) * dimension);
            }
            fin.close();
        }


        class Accessor {

            const Matrix &matrix_;
            std::vector<bool> flags_;
        public:
            typedef unsigned Key;
            typedef const T *Value;
            typedef T DATATYPE;

            explicit Accessor(const Matrix &matrix): matrix_(matrix) {
                flags_.resize(matrix_.getSize());
            }
            void reset() {
                flags_.clear();
                flags_.resize(matrix_.getSize());
            }
            bool mark(unsigned key) {
                if (flags_[key]) {
                    return false;
                }
                flags_[key] = true;
                return true;
            }
            const T *operator () (unsigned key) const {
                return matrix_[key];
            }
        };

    };


    template <class T>
    class Visitor {

    private:
        const Matrix<T> & _matrix;
        const int         _offset;
        const int         _dimension;
    public:
        Visitor(const Matrix<T> & matrix, const int offset, const int dimension)
                : _matrix(matrix), _offset(offset), _dimension(dimension) {}

        const T *operator [] (int i) const {
            return _matrix[i] + _offset;
        }

        int getDim() const {
            return _dimension;
        }

        int getSize() const {
            return _matrix.getSize();
        }
    };
}
