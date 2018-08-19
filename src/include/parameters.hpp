//////////////////////////////////////////////////////////////////////////////
/// Copyright 2018-present      DAI<    .dai@outlook.com>
///
/// permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
/// documentation files (the "Software"), to deal in the Software without restriction, including without limitation
/// the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
/// and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in all copies or substantial portions of
/// the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
/// THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
/// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.

/// @version 0.1
/// @author       DAI
/// @contact     .dai@outlook.com
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <iostream>
#include <boost/program_options.hpp>

namespace ss{


struct parameter {
    std::string train_data;
    std::string base_data;
    std::string query_data;
    std::string ground_truth;

    int topK;
    int num_thread;
    int dim;
    int transformed_dim;
    int origin_dim;

    int num_bit;
    int num_sub_data_set;

    int train_size;
    int query_size;
    int base_size;

    float r; // h(x) = ceil[(av+b)/r]

} typedef parameter;

}
