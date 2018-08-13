#include <iostream>
#include <vector>

#include <boost/program_options.hpp>

#include "include/parameters.hpp"
#include "include/index.hpp"
#include "include/index/srp.hpp"
#include "include/query/hrquery.hpp"

#include "include/bench/bencher.h"
#include "include/bench/benchrecord.h"

#include "include/matrix.h"
#include "include/utils/util.hpp"

#include "executor.hpp"

using namespace std;
using namespace ss;
using namespace lshbox;


int main(int argc, char** argv) {
	using DataType = float;
	parameter para;
	load_options(argc, argv, para);
	execute<DataType, SRPIndex<DataType >, HRQuery<DataType > >(para);	
}

