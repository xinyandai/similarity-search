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
#include "include/index/norm_range.hpp"
#include "include/query/norm_range.hpp"

using namespace std;
using namespace ss;
using namespace lshbox;


int main(int argc, char** argv) {
	using DataType = float;
	parameter para;
	load_options(argc, argv, para);
	execute<DataType, NormRangeIndex<DataType >, NormRangeQuery<DataType > >(para, IP_DIST);	
}

