#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>
#include <utility>
#include "benchrecord.hpp"

using std::vector;
using std::pair;
using std::cout;
using std::endl;
using std::ifstream;
using std::string;
using std::istringstream;

class Bencher {
private:
    vector<BenchRecord> _knns;
    int                 _topK;
    int                 _queries;
public:
    int getTopK()    const { return _topK; }
    int getQueries() const { return _queries; }

    explicit Bencher (const char* benchFile) {
        ifstream reader(benchFile);
        if (!benchFile) {
            cout << "cannot open benchFile: " << benchFile << endl;
            assert(false);
        }

        string line;
        getline(reader, line);
        istringstream line_string(line);
        line_string >>_queries >> _topK;

        _knns.reserve(_queries);

        for (int i = 0; i < _queries; ++i) {
            int query_id;
            int item_id;
            float distance;

            getline(reader, line);
            istringstream knn_stream(line);

            vector<pair<float, int>> record;
            record.reserve(_topK);

            knn_stream >> query_id;
            while(knn_stream >> item_id >> distance) {
                record.emplace_back(std::make_pair(distance, item_id));
            }
            _knns.emplace_back(BenchRecord(query_id, record, true));
        }
        reader.close();
    }

    explicit Bencher (const vector<vector<pair<float, int>>>& source, bool is_sorted = false) {
        int num_queries = source.size();
        _knns.reserve(num_queries);

        for (int query_id = 0; query_id < num_queries; ++query_id) {
            _knns.emplace_back(BenchRecord(query_id, source[query_id], is_sorted));
        }
    }

    int size() const {
        return this->_knns.size();
    }

    float avg_recall(const Bencher& given) const {
        assert(this->size() >= given.size());

        int size = std::min(this->size(), given.size());
        float sum_recall = 0;
        for (int i = 0; i < size; ++i) {
            sum_recall += this->_knns[i].recall(given.getRecord(i));
        }
        return sum_recall / size;
    }

    float avg_precision(const Bencher& given, const vector<int>& numItemProbed) const {
        assert(this->size() >= given.size());

        int size = std::min(this->size(), given.size());
        float sum_precision = 0;
        for (int i = 0; i < size; ++i) {
            sum_precision += this->_knns[i].precision(given.getRecord(i), numItemProbed[i]);
        }
        return sum_precision / size;
    }

    float avg_error(const Bencher& given) const {
        assert(this->size() >= given.size());
        float sum_error = 0;
        int count = 0;

        int size = std::min(this->size(), given.size());
        for (int i = 0; i < size; ++i) {

            float error = this->_knns[i].error(given.getRecord(i));

            if (error >= 1) {
                sum_error += error;
                count++;
            }
        }
        if (count == 0)
            return -1;
        else
            return sum_error / count;
    }

    float avg_items(const  vector<int>& num_item_probed) {
        if (num_item_probed.size() == 0) return 0;
        uint64_t sum = 0;
        for (const int& v : num_item_probed) {
            sum += v;
        }
        double result = sum / static_cast<double>(num_item_probed.size());
        return  static_cast<float>(result);
    }

    const BenchRecord& getRecord(int qId) const {
        return this->_knns[qId];
    }
};
