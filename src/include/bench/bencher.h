/*
 * Bencher loads benchfile.lshbox and provides evaluations
 * */
#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>
#include <utility>
#include "benchrecord.h"
using std::vector;
using std::pair;
using std::cout;
using std::endl;
using std::ifstream;
using std::string;
using std::istringstream;

class Bencher {
private:
    vector<BenchRecord> nns;
    int topK;
    int queries;
public:
    int getTopK()	const { return topK; }
    int getQueries()	const { return queries; }

    Bencher (const char* benchFile) {
        ifstream fin(benchFile);
        if (!benchFile) {
            cout << "cannot open benchFile: " << benchFile << endl;
            assert(false);
        }

        string line;
        getline(fin, line);
        istringstream issFirstLine(line);
        issFirstLine >>queries >> topK;
	

        nns.reserve(queries);
        unsigned qid;
        unsigned itemId;
        float itemDist;
        for (int i = 0; i < queries; ++i) {
            getline(fin, line);
            istringstream iss(line);
            iss >> qid;
            vector<pair<float, unsigned>> record;
            while(iss >> itemId >> itemDist) {
                record.emplace_back(std::make_pair(itemDist, itemId));
            }
            nns.emplace_back(BenchRecord(qid, record, true));
        }
        fin.close();
    }

    Bencher (const vector<vector<pair<float, unsigned>>>& source, bool isSorted = false) {
        int numQueries = source.size();
        nns.reserve(numQueries);

        unsigned qid;
        for (int i = 0; i < numQueries; ++i) {
            qid = i;
            nns.emplace_back(BenchRecord(qid, source[i], isSorted));
        }
    }

    unsigned size() const {
        return this->nns.size();
    }

    float avg_recall(const Bencher& given) const {
        assert(this->size() >= given.size());

        unsigned size = std::min(this->size(), given.size());
        float sumRecall = 0;
        for (int i = 0; i < size; ++i) {
            const BenchRecord& givenRecord = given.getRecord(i);
            sumRecall += this->nns[i].recall(givenRecord);
        }
        return sumRecall / size;
    }

    float avg_precision(const Bencher& given, const vector<unsigned>& numItemProbed) const {
        assert(this->size() >= given.size());

        unsigned size = std::min(this->size(), given.size());
        float sumPrecision = 0;
        for (int i = 0; i < size; ++i) {
            const BenchRecord& givenRecord = given.getRecord(i);
            sumPrecision += this->nns[i].precision(givenRecord, numItemProbed[i]);
        }
        return sumPrecision / size;
    }

    float avg_error(const Bencher& given) const {
        assert(this->size() >= given.size());
        float sumError = 0;
        int count = 0;

        int size = std::min(this->size(), given.size());
        for (int i = 0; i < size; ++i) {
            const BenchRecord& givenRecord = given.getRecord(i);;
            float er = this->nns[i].error(givenRecord);
            if (er >= 1){ 
                sumError += er;
                count++;
            }
        }
        if (count == 0) return -1;
        else return sumError / count;
    }

    float avg_items(const  vector<unsigned>& numItemProbed) {
    	if (numItemProbed.size() == 0) return 0;
	unsigned long long sum = 0;
	for (const unsigned& v : numItemProbed) {
		sum += v;
	}
	double result = (double) sum / numItemProbed.size();
	return ((float)result);
    }

    const BenchRecord& getRecord(unsigned qId) const {
        return this->nns[qId];
    }
};
