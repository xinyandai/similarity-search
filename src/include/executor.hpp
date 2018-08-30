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
#include "matrix.hpp"
#include "metric.hpp"
#include "utils/util.hpp"

using namespace std;
using namespace ss;

void LoadOptions(int argc, char **argv, parameter &para) {
    namespace po = boost::program_options;

    po::options_description opts("Allowed options");
    opts.add_options()
        ("help,h", "help info")
        ("num_bit,l",        po::value<int   >(&para.num_bit)         ->default_value(32)  , "num of hash bit")
        ("graph_K",          po::value<int   >(&para.graph_K)         ->default_value(50)  , "number of neighbors")
        // TODO(Xinyan): to support multi thread
        ("num_thread",       po::value<int   >(&para.num_thread)      ->default_value(1),    "num of thread")
        ("dim,d",            po::value<int   >(&para.dim)             ->default_value(-1),   "origin dimension of data")
        ("transformed_dim",  po::value<int   >(&para.transformed_dim) ->default_value(0)  ,  "origin dimension of data")
        ("num_sub_data_set", po::value<int   >(&para.num_sub_data_set)->default_value(-1),   "number of sub data set")
        ("r,r",              po::value<float >(&para.r)               ->default_value(2.5),  "float 'w' in e2lsh")
        ("kmeans_centers",   po::value<int   >(&para.kmeans_centers)  ->default_value(50),   "number of centers")
        ("num_codebook",     po::value<int   >(&para.num_codebook)    ->default_value(2),    "number of codebook")
        ("iteration,i",      po::value<int   >(&para.iteration)       ->default_value(10),   "training iterations")

        ("train_data,t",     po::value<string >(&para.train_data),                         "data for training")
        ("base_data,b",      po::value<string >(&para.base_data) ,                         "data stored in index")
        ("query_data,q",     po::value<string >(&para.query_data),                         "data for query")
        ("ground_truth,g",   po::value<string >(&para.ground_truth),                       "ground truth file")
        ("graph_knn",        po::value<string >(&para.graph_knn),                          "k nearest neighbors")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, opts), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << opts << endl;
        exit(0);
    }
}

template <class DataType, class IndexType, class QueryType, class MetricType=ss::EuclidMetric<DataType> >
int SearchIterative(parameter &para) {
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


    vector<QueryType * >                  queries(para.query_size);
    for (int i = 0; i < para.query_size; i++) {
        /// determine the sequence of probing bucket(except probing by looking up)
        queries[i] = (new QueryType(&index, query_data[i], metric, base_data, para) );
    }

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

    for (int num_items = 1; num_items / 2 < para.base_size; num_items *= 2 ) {

        if ( num_items > para.base_size )
            num_items = para.base_size;

        vector<vector<pair<float, int > > >   current_topK(para.query_size);    //TODO copy should be avoided
        vector<int >                          item_probed(para.query_size);

        {  /// probing

            for (int i = 0; i <  para.query_size; i++) {  /// probing
                /// probe more bucket util the number of probed item is no less than {$numItems}
                queries[i]->ProbeItems(num_items);
                item_probed[i]  = queries[i]->GetNumItemsProbed();
                current_topK[i] = queries[i]->GetSortedTopK();
            }

        }

        /// statistic such as recall, precision, probing time and probed items
        double timer_elapsed = time_recorder.elapsed();

        {
            Bencher current_bench(current_topK, true);
            cout << num_items                                                << spliter
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
