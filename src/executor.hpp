#include <iostream>
#include <vector>

#include <boost/program_options.hpp>

#include "include/parameters.hpp"
#include "include/index.hpp"

#include "include/bench/bencher.h"
#include "include/bench/benchrecord.h"

#include "include/matrix.h"
#include "include/utils/util.hpp"

using namespace std;
using namespace ss;
using namespace lshbox;


void load_options(int argc, char** argv, parameter& para) {
	
	namespace po = boost::program_options;

	po::options_description opts("Allowed options");
	opts.add_options()
		("help,h", "help info")

		("num_bit,l",		po::value<int 	>(&para.num_bit)->default_value(32)  , "num of hash bit")
		("num_thread",		po::value<int 	>(&para.num_thread)->default_value(1), "num of thread")	//TODO to support multi thread
		("dim,d", 		po::value<int 	>(&para.dim)->default_value(-1)      , "origin dimension of data")
		("transformed_dim",	po::value<int 	>(&para.transformed_dim)->default_value(0)      , "origin dimension of data")
		("num_sub_data_set",	po::value<int 	>(&para.num_sub_data_set)->default_value(-1), "number of sub data set")
		("r,r",			po::value<float >(&para.r)->default_value(0.8), "quantization float 'w' in e2lsh, h(x)=ceil[(av+b)/w]")
		
		("train_data,t",  	po::value<string >(&para.train_data),   "data for training")
		("base_data,b",		po::value<string >(&para.base_data) ,   "data stored in index")
		("query_data,q",	po::value<string >(&para.query_data),   "data for query")
		("ground_truth,g",	po::value<string >(&para.ground_truth), "ground truth file")
	;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, opts), vm);
	po::notify(vm);

	para.map = vm;

	if(vm.count("help")) {
		cout << opts << endl; 
		exit(0);
	}
}

template <class DataType, class IndexType, class QueryType>
int execute(parameter& para, int distance_metric) {
	
	ss::timer time_recorder;

	cout	<< "==============================================================================" << endl;
	cout	<< "==============================================================================" << endl;
	cout << "[loading ] loading ground truth " << endl;
	Bencher truthBencher(para.ground_truth.c_str());	

	cout << "[loading ] loading fvecs data                                    " ;
	time_recorder.restart();
	lshbox::Matrix<DataType> train_data(para.train_data);
	lshbox::Matrix<DataType> base_data (para.base_data);
	lshbox::Matrix<DataType> query_data(para.query_data);
	cout << "using time :" << time_recorder.elapsed() << endl;

	para.topK = truthBencher.getTopK();	
	para.train_size = train_data.getSize();
	para.base_size  = base_data.getSize();
	para.query_size = query_data.getSize();
	para.dim = train_data.getDim() + para.transformed_dim;
	para.origin_dim = train_data.getDim();

	cout << "[training] initial the index." << endl;
	IndexType * index = new IndexType(para);

	cout << "[training] training the hash functions or models                 ";
	time_recorder.restart();
	index->train(train_data); 	// train the model, generate random vectors if using LSH, learn the projection vectors if using larning to hash method.(PCAH, ITQ, SH ...)
	cout << "using time :" << time_recorder.elapsed() << endl;

	cout << "[training] put data into index                                   ";
	time_recorder.restart();
	index->add(base_data);		// add database items into index, using the information(e.g. hahs function) learned/generated in training stage.
	cout << "using time :" << time_recorder.elapsed() << endl;

	lshbox::Metric<DataType > 		metric(train_data.getDim(), distance_metric);
	typename Matrix<DataType >::Accessor 	accessor(base_data);
	
	vector<QueryType * > 			queries;

	
	cout << "[querying] rank buckets for each query                           ";
	time_recorder.restart();
	for (int i = 0; i < para.query_size; i++) {
		queries.push_back(new QueryType(index, query_data[i], metric, accessor, para ) ); // determine the sequence of probing bucket(except probing by looking up)
	}
	cout << "using time :" << time_recorder.elapsed() << endl;

	const char * spliter = ", ";
	cout	<< "==============================================================================" << endl;
	cout	<< "==============================================================================" << endl;
	cout	
		<< "expected items" << spliter
		<< "avg items" << spliter 
		<< "overall time" << spliter 
		<< "avg recall" << spliter 
		<< "avg precision" << spliter
		<< "avg error" 
		<< "\n";

	time_recorder.restart();
	for (int numItems = 1; numItems/2 < para.base_size; numItems *=2 ) {
		
		if ( numItems > para.base_size ) 	
			numItems = para.base_size;

		vector<vector<pair<float, unsigned>>> 	currentTopK(para.query_size);	//TODO copy should be avoided
		vector<unsigned> 			numItemProbed(para.query_size);

		for (int i = 0; i <  para.query_size; i++) {
			queries[i]->probeItems(numItems);				// probe more bucket util the number of probed item is no less than {$numItems}
			numItemProbed[i] 	= queries[i]->getNumItemsProbed();
			currentTopK[i] 		= queries[i]->getSortedTopK();
		}

		// statistic such as recall, precision, probing time and probed items
		Bencher currentBencher(currentTopK, true);
		cout
			<< numItems <<  spliter 
		        << truthBencher.avg_items(numItemProbed) << spliter 
			<< time_recorder.elapsed() << spliter 
		        << truthBencher.avg_recall(currentBencher) << spliter 
		        << truthBencher.avg_precision(currentBencher, numItemProbed) << spliter 
		        << truthBencher.avg_error(currentBencher)
		       	<< "\n";
	}

	delete index;
	for (int i =0 ; i < para.query_size; i++) {
		delete queries[i];
	}

	return 0;
}
