#include <iostream>
#include <vector>

#include <boost/program_options.hpp>

#include "include/parameters.hpp"
#include "include/bench/bencher.h"
#include "include/bench/benchrecord.h"
#include "include/matrix.h"
#include "include/metric.h"
#include "include/utils/util.hpp"

#include "executor.hpp"
#include "include/index/simple_lsh.hpp"
#include "include/index/srp.hpp"
#include "include/query/hamming_ranker.hpp"

using namespace std;
using namespace ss;
using namespace lshbox;


int main(int argc, char** argv) {

	using DataType = float;
	using KeyType  = unsigned long long;
	using IndexType =  ss::SimpleLSHIndex<DataType, KeyType, ss::SRPIndex<DataType > >;
	using QueryType =  ss::HammingRanker<DataType >;
	
	parameter para;
	load_options(argc, argv, para);
	execute<DataType, IndexType, QueryType > (para, IP_DIST);
}

