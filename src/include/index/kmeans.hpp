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

#include <utility>
#include <vector>
#include <random>
#include <unordered_map>
#include <boost/progress.hpp>

#include "../index.hpp"

namespace ss {

    using namespace std;

    template<class DataType>
    class KMeansIndex : public Index<DataType> {
    protected:
        /// _center[i] means the i'th center's coordinate
        vector<vector<DataType > >     _centers;
        /// _points[i] means all points' indexes belong to i'th center
        /// _points[i][j] means the j'th point's index belongs to i'th center
        vector<vector<int > >          _points;
    public:
        explicit KMeansIndex(const parameter& para) :
                Index<DataType >(para),
                _centers(para.kmeans_centers),
                _points(para.kmeans_centers) {}


        const vector<vector<DataType > > & get_centers() const { return _centers; }
        const vector<vector<int > >      & get_points()  const { return _points; }


        void Train(const lshbox::Matrix<DataType > & data) override {
            iterate(lshbox::Visitor<DataType >(data, 0, data.getDim()));
        }


        void Add(const lshbox::Matrix<DataType > & data) override {
            Assign(lshbox::Visitor<DataType >(data, 0, data.getDim()));
        }


        /***
         * iteratively update centers and re-assign points
         * @param data
         */
        void iterate(const lshbox::Visitor<DataType> & data) {

            /// initialize centers
            /// TODO(Xinyan): should be randomly initialize
            for(int i=0; i<_centers.size(); i++) {
                _centers[i] = vector<DataType >(data[i], data[i]+data.getDim());
            }

            boost::progress_display progress(this->_para.iteration);
            for (int iter = 0; iter < this->_para.iteration; ++iter, ++progress) {

                /// Assignment
                Assign(data);
                /// UpdateCenter
                Update(data);
                /// clear points in each centers
                for (int c = 0; c<_points.size(); c++) {
                    _points[c].clear();
                }
            }
        }

        /**
         * re-calculate center by averaging points' coordinate
         */
        void Update(const lshbox::Visitor<DataType> & data) {
            for (int c = 0; c < _centers.size(); ++c) {

                vector<DataType > sum(data.getDim(), 0.0f);
                for (int p = 0; p < _points[c].size(); ++p) {

                    for (int d = 0; d < data.getDim(); ++d) {
                        sum[d] += data[_points[c][p]][d];        /// add up
                    }
                }

                for (int d = 0; d < data.getDim(); ++d) {
                    _centers[c][d] = sum[d] / _points[c].size(); /// average
                }
            }
        }

        /**
         * assign each point in {@link data} to nearest center
         */
        void Assign(const lshbox::Visitor<DataType> & data) {
            for (int i=0; i<data.getSize(); ++i) {

                _points[NearestCenter(data[i], data.getDim())].push_back(i);
            }
        }


        int NearestCenter(const DataType * vector, int dimension) {
            DataType min_distance = ss::EuclidDistance(vector, _centers[0].data(), dimension);
            int nearest_center = 0;
            for (int c = 1; c < _centers.size(); c++) {
                DataType distance = Distance(vector, dimension, c);
                if (distance < min_distance) {
                    min_distance = distance;
                    nearest_center = c;
                }
            }
            return nearest_center;
        }

        /// I am not expected to understand why it cannot be compiled using vector<pair> directly.
        typedef vector< pair<float, int > > VectorPair;
        /**
         * calculate distances from {@link vector} to each center
         * @return distances within a vector of pair<distance, center>
         */
        VectorPair ClusterDistance(const DataType *vector, int dimension) {
            VectorPair dist_centers(this->_para.kmeans_centers);
            for (int center = 0; center < this->_para.kmeans_centers; ++center) {
                DataType distance = Distance(vector, dimension, center);
                dist_centers[center] = std::make_pair(distance, center);
            }

            return dist_centers;
        };

        inline DataType Distance(const DataType * vector, int dimension, int center) {
            return ss::EuclidDistance(vector, _centers[center].data(), dimension);
        }

    };

}
