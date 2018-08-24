////////////////////////////////////////////////////////////////////////////////
/// Copyright 2018-present Xinyan DAI<xinyan.dai@outlook.com>
///
/// permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to
/// deal in the Software without restriction, including without limitation the
/// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
/// sell copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions ofthe Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
/// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
/// IN THE SOFTWARE.

/// @version 0.1
/// @author  Xinyan DAI
/// @contact xinyan.dai@outlook.com
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
#include <algorithm>
#include <utility>
#include <unordered_set>

using std::vector;
using std::pair;
using std::unordered_set;

class BenchRecord {
public:
    
    BenchRecord(int qId, const vector<pair<float, int>>& nbs, bool sorted = false) {
        // initialized queryId
        this->queryId = qId;

        // initialized knn
        if (sorted) {
            this->knn = nbs;
        } else {
            std::sort(this->knn.begin(), this->knn.end(),
                [](const pair<float, int>& a, const pair<float, int>& b) {
                    if (a.first != b.first)
                        return a.first < b.first;
                    else 
                        return a.second < b.second;
                });
            this->knn = nbs;
        }

        // initialized knnIvecs
        this->knnIvecs.reserve(this->knn.size());
        for (const auto& p : this->knn) {
            this->knnIvecs.insert(p.second);
        }
    }

    void push_back(int nbid, float dist) {
        assert(dist >= this->knn.back().first);
        this->knn.push_back(std::make_pair(dist, nbid));
        this->knnIvecs.insert(nbid);
    }

    int getId() const {
        return this->queryId;
    }

    const vector<pair<float, int>>& getKNN() const {
        return this->knn;
    }
    
    int size() {
        return this->knn.size();
    }

    float precision(const BenchRecord& other, int numProbed) const {
        return precision(other.getId(), other.getKNN(), numProbed);
    }

    float recall(const BenchRecord& other) const {
        return recall(other.getId(), other.getKNN());
    }

    float error(const BenchRecord& other) const {
        return error(other.getId(), other.getKNN());
    }

private:
    int queryId;
    vector<pair<float, int>> knn;
    unordered_set<int> knnIvecs;

    float precision(int qId, const vector<pair<float, int>>& givenKNN, int numProbed) const {
        int numMatched = numRetrieved(qId, this->extractIvecs(givenKNN));
        return static_cast<float>(numMatched / numProbed);
    }

    float recall(int qId, const vector<pair<float, int>>& givenKNN) const {
        int numMatched = numRetrieved(qId, this->extractIvecs(givenKNN));
        return numMatched / static_cast<float>(this->knn.size());
    }

    int numRetrieved(int qId, const vector<int>& ivecs) const {
        assert(this->queryId == qId);
        int matched = 0;
        for (vector<int>::const_iterator it = ivecs.begin(); it != ivecs.end(); ++it) {
            if (this->knnIvecs.find(*it) != this->knnIvecs.end()) {
                matched++;
            }
        }
        return matched;
    }

    float error(int qId, const vector<pair<float, int>>& givenKNN) const {
        if (givenKNN.size() == 0) return -1;

        float error = 0;
        int size = std::min(givenKNN.size(), this->knn.size());

        int idx = 0;
        int idxGiven = 0;
        int count = 0;
        while(idx < size && idxGiven < size) {
            if (this->knn[idx].first == 0 && givenKNN[idxGiven].first > 0.000001) {
                idx++;
                continue;
            }
            if (this->knn[idx].first > 0.000001 && givenKNN[idxGiven].first < 0.000001) {
                assert(false);
            }

            if (this->knn[idx].first < 0.000001 && givenKNN[idxGiven].first < 0.000001) {
                error += 1;
            } else {
                float thisError = givenKNN[idxGiven].first / this->knn[idx].first;
                error += thisError;
            }

            count++;
            idx++;
            idxGiven++;
        }
        
        if (count == 0) return -1;
        else return error / count;
    }

    vector<int> extractIvecs(const vector<pair<float, int>>& givenKNN) const {
        vector<int> ivecs;
        ivecs.resize(givenKNN.size());
        for (int i = 0; i < givenKNN.size(); ++i) {
            ivecs[i] = givenKNN[i].second;
        }
        return ivecs;
    }
};

