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
#include <assert.h>
#include <string.h>
#include <cmath>
#include <iostream>
#include <cstring>
namespace lshbox
{
/**
 * Dataset management class. A dataset is maintained as a matrix in memory.
 *
 * The file contains N D-dimensional vectors of single precision floating point numbers.
 *
 * Such binary files can be accessed using lshbox::Matrix<double>.
 */
template <class T>
class Matrix
{
    long dim;
    long N;
    T *dims;

    std::vector<T *> data;
    /** the number of vector each bucket contains */
    long bucketSize;
    const long max_alloc = 1024*1024*1024;
public:
    void free_() {
	for(auto it = data.begin(); it != data.end(); ++it) {
    	    delete [] (*it);
        }
     }

    void malloc_() {
    
        bucketSize = (long) (max_alloc / dim);  // how many vectors each bucket contains
        data.resize(1 + N / bucketSize);        // how many bucket we need
         
        for(int i=0; i<data.size()-1; i++) {
            data[i] = new T[bucketSize * dim]; // bucket initialize
	    std::memset(data[i], 0, sizeof(T) * dim * N);
        }   
        // the last bucket's size contains the rest vectors
	int rest = N  - bucketSize * (data.size() -1);
        data[data.size() - 1] = new T[rest * dim];
	std::memset(data[data.size() - 1], 0, sizeof(T) * dim * rest);
     }

    /**
     * Reset the size.
     *
     * @param _dim Dimension of each vector
     * @param _N   Number of vectors
     */
    void reset(int _dim, int _N)
    {
        dim = _dim;
        N = _N;

	    free_();
	    malloc_();
    }

    Matrix(): dim(0), N(0) {}
    Matrix(int _dim, int _N)
    {
        reset(_dim, _N);
    }
    ~Matrix()
    {
        free_();
    }
    /**
     * Access the ith vector.
     */
    const T *operator [] (int i) const
    {

	    int bucket = i / bucketSize;
	    int offset = i % bucketSize;
	    return data[bucket] + offset * dim;
    }
    /**
     * Access the ith vector.
     */
    T *operator [] (int i)
    {
    	int bucket = i / bucketSize;
	    int offset = i % bucketSize;
	    return data[bucket] + offset * dim;
    }
    /**
     * Get the dimension.
     */
    int getDim() const
    {
        return dim;
    }
    /**
     * Get the size.
     */
    int getSize() const
    {
        return N;
    }

    Matrix(const std::string &path, int transformed_dim): dims(NULL)
    {
        loadFvecs((*this), path, transformed_dim);
    }
    Matrix(const Matrix& M) = delete;
    Matrix& operator=(const Matrix& M)  = delete;

    std::vector<float> calNorms() {
        std::vector<float> results(this->getSize());
        float norm;
        for (int i = 0; i < results.size(); ++i) {
            norm = 0;
            for (int idx = 0; idx < this->getDim(); ++idx) {
                norm += (*this)[i][idx] * (*this)[i][idx];
            }
            results[i] = sqrt(norm);
        }
        return results;
    }


    /**
     * An accessor class to be used with LSH index.
     */
    class Accessor
    {
        const Matrix &matrix_;
        std::vector<bool> flags_;
    public:
        typedef unsigned Key;
        typedef const T *Value;
        typedef T DATATYPE;
        Accessor(const Matrix &matrix): matrix_(matrix)
        {
            flags_.resize(matrix_.getSize());
        }
        void reset()
        {
            flags_.clear();
            flags_.resize(matrix_.getSize());
        }
        bool mark(unsigned key)
        {
            if (flags_[key])
            {
                return false;
            }
            flags_[key] = true;
            return true;
        }
        const T *operator () (unsigned key) const
        {
            return matrix_[key];
        }
    };

    template<typename DATATYPE>
    friend void loadFvecs(Matrix<DATATYPE>& data, const std::string& dataFile, int transformed_dim) {
        std::ifstream fin(dataFile.c_str(), std::ios::binary | std::ios::ate);
        if (!fin) {
            std::cout << "cannot open file " << dataFile.c_str() << std::endl;
            assert(false);
        }
        unsigned long fileSize = fin.tellg();
        fin.seekg(0, fin.beg);
        assert(fileSize != 0);

        int dimension;
        fin.read((char*)&dimension, sizeof(int));
        unsigned bytesPerRecord = dimension * sizeof(DATATYPE) + 4;
        assert(fileSize % bytesPerRecord == 0);
        long cardinality = fileSize / bytesPerRecord;

        data.reset(dimension+transformed_dim, cardinality);
        fin.read((char *)(data[0]), sizeof(float) * dimension);

        int dim;
        for (int i = 1; i < cardinality; ++i) {
            fin.read((char*)&dim, sizeof(int));
            assert(dim == dimension);
            fin.read((char *)(data[i]), sizeof(float) * dimension);
        }
        fin.close();
    }

};
}
