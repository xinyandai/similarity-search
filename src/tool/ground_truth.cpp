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


#include <functional>
#include <vector>

#include "utils/ground_truth.hpp"
#include "matrix.hpp"

using namespace std;


int main(int argc, char** argv) {

    typedef float DataType;

    if (argc != 7 && argc != 8) {
        cout << "usage: program base_file.fvecs query_file.fvecs K groundtruth_file.lshbox groundtruth_file.ivecs num_threads=4" << endl;
        return 0;
    }

    const char* baseFileName = argv[1];
    const char* queryFileName = argv[2];
    int K = std::atoi(argv[3]);
    const char* lshboxBenchFileName = argv[4];
    const char* ivecsBenchFileName = argv[5];
    string metric = argv[6];
    int numThreads = 4;
    if (argc >= 8)
        numThreads = stoi(argv[7]);

    ss::Matrix<DataType> base_data (baseFileName);
    ss::Matrix<DataType> query_data(queryFileName);

    std::function<float(const DataType *, const DataType *, const int )> dist;
    if (metric == "euclid") {
        dist = ss::EuclidDistance<DataType >;
    } else if (metric == "angular"){
        dist = ss::AngularDistance<DataType >;
    } else if (metric == "product") {
        dist = ss::InnerProductDistance<DataType >;
    } else {
        assert(false);
    }

    vector<vector<MaxHeapElement<int> > > knn = ss::ExactKNN<DataType >(
            query_data[0],
            query_data.getSize(),
            base_data[0],
            base_data.getSize(),
            query_data.getDim(),
            K,
            dist);

    ss::GroundWriter::WriteLSHBOX<DataType>(lshboxBenchFileName, knn);
    ss::GroundWriter::WriteIVECS<DataType>(ivecsBenchFileName, knn);
    return 0;
}
