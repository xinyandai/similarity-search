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
#include <string>
#include <iostream>
#include <vector>
#include <utility>

#include <boost/program_options.hpp>

#include "bench/bencher.hpp"
#include "bench/benchrecord.hpp"

#include "parameters.hpp"
#include "index.hpp"
#include "executor.hpp"
#include "matrix.hpp"
#include "metric.hpp"
#include "utils/util.hpp"

using namespace std;
using namespace ss;


template <class DataType, class IndexType, class QueryType, class MetricType=ss::EuclidMetric<DataType> >
int Search(parameter &para) {
    ss::timer time_recorder;

    cout    << "#==============================================================================" << endl;
    cout    << "#==============================================================================" << endl;
    cout    << "#[loading ] loading ground truth " << endl;
    Bencher truth_bench(para.ground_truth.c_str());

    cout << "#[loading ] loading fvecs data                                    " ;
    time_recorder.restart();
    Matrix<DataType> train_data(para.train_data);
    Matrix<DataType> base_data (para.base_data);
    Matrix<DataType> query_data(para.query_data);
    cout << "using time :" << time_recorder.elapsed() << endl;

    para.topK       = truth_bench.getTopK();
    para.train_size = train_data.getSize();
    para.base_size  = base_data.getSize();
    para.query_size = query_data.getSize();
    para.dim        = train_data.getDim() + para.transformed_dim; /// useful when add dimensions for some algorithm
    para.origin_dim = train_data.getDim();

    MetricType metric(para.origin_dim);

    cout << "#[training] initial the index." << endl;
    IndexType index(para);

    cout << "#[training] training the hash functions or models                 ";
    time_recorder.restart();
    /// train the model, generate random vectors if using LSH,
    /// learn the projection vectors if using learning to hash method.(PCAH, ITQ, SH ...)
    index.Train(train_data);
    cout << "using time :" << time_recorder.elapsed() << endl;

    cout << "#[training] put data into index                                   ";
    time_recorder.restart();
    /// add database items into index, using the information(e.g. hahs function) learned/generated in training stage.
    index.Add(base_data);
    cout << "using time :" << time_recorder.elapsed() << endl;

    const char * spliter = ", ";
    cout << "#==============================================================================" << endl;
    cout << "#==============================================================================" << endl;
    cout << "expected items" << spliter
         << "overall time" << spliter
         << "avg recall" << spliter
         << "avg precision" << spliter
         << "avg error" << spliter
         << "avg items"
         << "\n";

    time_recorder.restart();

    vector<QueryType * >                  queries(para.query_size);
    for (int i = 0; i < para.query_size; i++) {
        /// determine the sequence of probing bucket(except probing by looking up)
        queries[i] = (new QueryType(&index, query_data[i], metric, base_data, para) );
    }

    {
        vector<vector<pair<float, int > > >   current_topK(para.query_size);    //TODO copy should be avoided
        vector<int >                          item_probed(para.query_size);

        {  /// probing

            for (int i = 0; i <  para.query_size; i++) {  /// probing
                /// probe more bucket util the number of probed item is no less than {$numItems}
                queries[i]->ProbeItems(-1);
                item_probed[i]  = queries[i]->GetNumItemsProbed();
                current_topK[i] = queries[i]->GetSortedTopK();
            }

        }

        /// statistic such as recall, precision, probing time and probed items
        double timer_elapsed = time_recorder.elapsed();

        {
            Bencher current_bench(current_topK, true);
            cout << -1                                                << spliter
                 << timer_elapsed                                            << spliter
                 << truth_bench.avg_recall(current_bench)                    << spliter
                 << truth_bench.avg_precision(current_bench, item_probed)    << spliter
                 << truth_bench.avg_error(current_bench)                     << spliter
                 << truth_bench.avg_items(item_probed)                       << "\n";
        }

    }

    for (int i = 0; i < para.query_size; i++) {
        delete  queries[i];
    }

    return 0;
}
