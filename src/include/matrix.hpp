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

#include <assert.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "utils/calculator.hpp"

namespace ss {

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
            loadFvecs(this, path);
        }
        Matrix(const Matrix& M) = delete;
        Matrix& operator=(const Matrix& M)  = delete;

        void reset(int _dim, int _N) {

            _dimension = _dim;
            _size = _N;
            _data = reinterpret_cast<T*>(std::realloc(_data, _dimension * _size * sizeof(T)));
        }

        const T *operator [] (int i) const {
            return _data + i * static_cast<size_t >(_dimension);/// i * dimension may overflow int
        }

        T *operator [] (int i) {
            return _data + i * static_cast<size_t >(_dimension);/// i * dimension may overflow int
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
        friend void loadFvecs(Matrix<DATATYPE> * data_point, const std::string& dataFile) {

            Matrix<DATATYPE>& data  = *data_point;

            std::ifstream fin(dataFile.c_str(), std::ios::binary | std::ios::ate);
            if (!fin) {
                std::cout << "cannot open file " << dataFile.c_str() << std::endl;
                assert(false);
            }
            uint64_t fileSize = fin.tellg();
            fin.seekg(0, fin.beg);
            assert(fileSize != 0);

            int dimension;
            fin.read(reinterpret_cast<char*>(&dimension), sizeof(int));
            unsigned bytesPerRecord = dimension * sizeof(DATATYPE) + 4;
            assert(fileSize % bytesPerRecord == 0);
            uint64_t cardinality = fileSize / bytesPerRecord;

            data.reset(dimension, cardinality);
            fin.read(reinterpret_cast<char*>(data[0]), sizeof(float) * dimension);

            int dim;
            for (int i = 1; i < cardinality; ++i) {
                fin.read(reinterpret_cast<char*>(&dim), sizeof(int));
                assert(dim == dimension);
                fin.read(reinterpret_cast<char*>(data[i]), sizeof(float) * dimension);
            }
            fin.close();
        }

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

} // namespace ss
