#pragma once

#include <vector>
#include <algorithm>
#include <utility>
#include <unordered_set>

using std::vector;
using std::pair;
using std::unordered_set;

class BenchRecord {
private:
    int                         _query_id;
    vector<pair<float, int> >   _knn;
    unordered_set<int>          _knn_ids;

public:
    BenchRecord(int query_id, const vector<pair<float, int>>& nbs, bool sorted = false) {

        // initialized queryId
        this->_query_id = query_id;
        this->_knn = nbs;

        if (!sorted) {
            std::sort(this->_knn.begin(),
                      this->_knn.end(),
                      [](const pair<float, int>& a, const pair<float, int>& b) {
                          if (a.first != b.first)
                              return a.first < b.first;
                          else
                              return a.second < b.second;
                      });
        }

        // initialized knn
        this->_knn_ids.reserve(this->_knn.size());
        for (const auto& p : this->_knn) {
            this->_knn_ids.insert(p.second);
        }
    }

    int getId() const {
        return this->_query_id;
    }

    const vector<pair<float, int>>& getKNN() const {
        return this->_knn;
    }
    
    int size() {
        return this->_knn.size();
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


    float precision(int qId, const vector<pair<float, int>>& givenKNN, int num_probed) const {
        int num_matched = numRetrieved(qId, (givenKNN));
        return num_matched / static_cast<float>(num_probed);
    }

    float recall(int qId, const vector<pair<float, int>>& givenKNN) const {
        int num_matched = numRetrieved(qId, (givenKNN));
        return num_matched / static_cast<float>(this->_knn.size());
    }

    int numRetrieved(int qId, const vector<pair<float, int> > & knn) const {
        assert(this->_query_id == qId);
        int matched = 0;
        for (vector<pair<float, int> >::const_iterator it = knn.begin(); it != knn.end(); ++it) {
            if (this->_knn_ids.find(it->second) != this->_knn_ids.end()) {
                matched++;
            }
        }
        return matched;
    }

    float error(int qId, const vector<pair<float, int>>& givenKNN) const {
        if (givenKNN.size() == 0) return -1;

        float error = 0;
        int size = std::min(givenKNN.size(), this->_knn.size());

        int idx = 0;
        int idxGiven = 0;
        int count = 0;
        while(idx < size && idxGiven < size) {
            if (this->_knn[idx].first == 0 && givenKNN[idxGiven].first > 0.000001) {
                idx++;
                continue;
            }
            if (this->_knn[idx].first > 0.000001 && givenKNN[idxGiven].first < 0.000001) {
                assert(false);
            }
            if (this->_knn[idx].first < 0.000001 && givenKNN[idxGiven].first < 0.000001) {
                error += 1;
            } else {
                float thisError = givenKNN[idxGiven].first / this->_knn[idx].first;
                error += thisError;
            }

            count++;
            idx++;
            idxGiven++;
        }
        
        if (count == 0)
            return -1;
        else
            return error / count;
    }
};

