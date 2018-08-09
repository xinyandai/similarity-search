#include <iostream>
#include <vector>

#include <boost/program_options.hpp>

#include "include/common/parameters.hpp"
#include "include/common/index.hpp"
#include "include/common/query.hpp"

#include "include/lshbox/eval.h"
#include "include/lshbox/bench/bencher.h"
#include "include/lshbox/bench/benchrecord.h"

#include "include/lshbox/matrix.h"
#include "include/common/basis.h"

using namespace std;
using namespace ss;
using namespace lshbox;

namespace po = boost::program_options;


int main(int argc, char** argv) {
	
	using DataType = float;

	ss::parameter para;

	po::options_description opts("Allowed options");
	opts.add_options()
		("help,h", "help info")

		("num_bit,nb", 	   po::value<int >(&para.num_bit)->default_value(32)  , "num of hash bit")
		("topK,k", 	   po::value<int >(&para.topK)->default_value(20)     , "size of result set")
		("num_thread,nt",  po::value<int >(&para.num_thread)->default_value(1), "num thread")
		("dim,d", 	   po::value<int >(&para.dim)->default_value(-1)      , "origin dimension of data")
		
		("train_data,td",  po::value<string >(&para.train_data),   "data for training")
		("base_data,bd",   po::value<string >(&para.base_data) ,   "data saved in index")
		("query_data,qd",  po::value<string >(&para.query_data),   "data for query")
		("ground_truth,gt",po::value<string >(&para.ground_truth), "data for query")
	;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, opts), vm);
	po::notify(vm);

	if(vm.count("help")) {
		cout << opts << endl; 
		return 0;
	}

	Bencher truthBencher(para.ground_truth.c_str());	

	cout
		<< "para.train_data " << para.train_data  << endl
		<< "para.base_data  " << para.base_data   << endl
		<< "para.query_data " << para.query_data  << endl
		<< endl;

	lshbox::Matrix<DataType> train_data(para.train_data);
	lshbox::Matrix<DataType> base_data(para.base_data);
	lshbox::Matrix<DataType> query_data(para.query_data);
	
	cout 
		<< "train_data dimension: \t" << train_data.getDim() << "\t size:" << train_data.getSize() << endl 
		<< "base_data dimension : \t" << base_data.getDim()  << "\t size:" << base_data.getSize()  << endl 
		<< "query_data dimension: \t" << query_data.getDim() << "\t size:" << query_data.getSize() << endl 
		<< endl	;

	para.train_size = train_data.getSize();
	para.base_size  = base_data.getSize();
	para.query_size = query_data.getSize();

	Index<DataType > * index = new Index<DataType >(para);

	index->preprocess_train(train_data);
	index->preprocess_base(base_data);
	index->preprocess_query(query_data);
	index->train(train_data);
	index->add(base_data);

	lshbox::Metric<DataType > metric(para.dim, L2_DIST);	
	typename lshbox::Matrix<DataType >::Accessor accessor(base_data);

	vector<Query<DataType>* > queries;
	for (int i = 0; i < para.query_size; i++) {
		queries.push_back(new Query<DataType>(index, query_data, metric, accessor, para ) );
	}

	cout 
		<< "expected items, " 
		<< "overall query time, "
		<< "avg recall, " 
		<< "avg precision, " 
		<< "avg error, " 
		<< "actual avg items" 
		<< "\n";

	ss::timer time_recorder;
	for (int numItems = 1; numItems < para.base_size; numItems *=2 ) {
		
		if ( numItems > para.base_size ) 
			numItems = para.base_size;
		
		vector<vector<pair<float, unsigned>>> currentTopK;
		currentTopK.reserve(para.query_size);
		
		vector<unsigned> numItemProbed;
		numItemProbed.reserve(para.query_size);

		for (int i = 0; i <  para.query_size; i++) {
			queries[i]->probeItems(numItems);
			numItemProbed.emplace_back(queries[i]->getNumItemsProbed());
			currentTopK.emplace_back(queries[i]->getSortedTopK());
		}

		// statistic such as recall precision
		Bencher currentBencher(currentTopK, true);
		cout 
			<< numItems << ", " 
			<< time_recorder.elapsed() <<", "
		        << truthBencher.avg_recall(currentBencher)<< ", "
		        << truthBencher.avg_precision(currentBencher, numItemProbed)<< ", "
		        << truthBencher.avg_error(currentBencher)<< ", "
		        << truthBencher.avg_items(numItemProbed) <<"\n";
	}

	delete index;
	for (int i =0 ; i < para.query_size; i++) {
		delete queries[i];
	}

	return 0;
}
