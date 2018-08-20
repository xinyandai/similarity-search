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

#include "int_index.hpp"

namespace ss {

    using namespace std;

    template<class DataType>
    class KMeansIndex : public Index<DataType> {
    protected:
        vector<vector<DataType > >     _centers;
        vector<vector<int > >          _points;
    public:
        explicit KMeansIndex(const parameter& para) :
                Index<DataType >(para),
                _centers(para.kmeans_centers),
                _points(para.kmeans_centers) {}


        const vector<vector<DataType > > & get_centers() const { return _centers; }
        const vector<vector<int > >      & get_points()  const { return _points; }

        void Train(const lshbox::Matrix<DataType > & data) override {

            for(int i=0; i<_centers.size(); i++) {
                _centers[i] = vector<DataType >(data[i], data[i]+data.getDim());
            }
        }

        void Add(const lshbox::Matrix<DataType > & data) override {
            /// TODO(XINYAN) : move vector operation to calculator
            boost::progress_display progress(this->_para.iteration * data.getSize());
            for (int iter = 0; iter < this->_para.iteration; ++iter) {
                // Assignment
                for (int i=0; i<data.getSize(); ++i && ++progress) {

                    DataType min_distance = ss::EuclidDistance(data[i], _centers[0].data(), data.getDim());
                    int nearest_center = 0;
                    for (int c = 1; c < _centers.size(); c++) {
                        DataType distance = ss::EuclidDistance(data[i], _centers[c].data(), data.getDim());
                        if (distance < min_distance) {
                            min_distance = distance;
                            nearest_center = c;
                        }
                    }

                    _points[nearest_center].push_back(i);
                }

                // UpdateCenter
                for (int c = 0; c < _centers.size(); ++c) {

                    vector<DataType > sum(data.getDim(), 0.0f);

                    for (int p = 0; p < _points[c].size(); ++p) {
                        for (int d = 0; d < data.getDim(); ++d) {
                            sum[d] += data[_points[c][p]][d];
                        }
                    }

                    for (int d = 0; d < data.getDim(); ++d) {
                        _centers[c][d] = sum[d] / _points[c].size();
                    }
                }
            }
        }

    };

}
