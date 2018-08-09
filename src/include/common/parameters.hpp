#pragma once

#include <iostream>
namespace ss{

struct parameter {
	std::string train_data;
	std::string base_data;
	std::string query_data;
	std::string ground_truth;

	int topK;
	int num_thread;
	int dim;

	int num_bit;

	int train_size;
	int query_size;
	int base_size;

} typedef parameter;

}
