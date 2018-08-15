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
#include "include/index/e2lsh.hpp"
#include "include/query/int_ranker.hpp"

using namespace std;
using namespace ss;
using namespace lshbox;


int main(int argc, char** argv) {
	using DataType = float;
	parameter para;
	load_options(argc, argv, para);
	execute<DataType, ss::E2LSHIndex<DataType >, ss::IntRanker<DataType > >(para, L2_DIST);	
}

