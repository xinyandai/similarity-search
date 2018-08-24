#include <vector>
#include <string>
#include <fstream>

#include "utils/ground_truth.h"
#include "include/matrix.h"

using namespace std;

class GroundWriter {
public:
    template<typename DataType>
    void writeLSHBOX(const char* lshboxBenchFileName, const vector<vector<MaxHeapElement<int> > >& knn) {
        // lshbox file
        ofstream lshboxFout(lshboxBenchFileName);
        if (!lshboxFout) {
            cout << "cannot create output file " << lshboxBenchFileName << endl;
            assert(false);
        }
        int K = knn[0].size();
        lshboxFout << knn.size() << "\t" << K << endl;
        for (int i = 0; i < knn.size(); ++i) {
            assert(knn[i].size() == K);
            lshboxFout << i << "\t";
            const vector<MaxHeapElement<int>>& topker = knn[i];
            for (int idx = 0; idx < topker.size(); ++idx) {
                lshboxFout << topker[idx].data() << "\t" << topker[idx].dist() << "\t";
            }
            lshboxFout << endl;
        }
        lshboxFout.close();
        cout << "lshbox groundtruth are written into " << lshboxBenchFileName << endl;
    }

    template<typename DataType>
    void writeIVECS(const char* ivecsBenchFileName, const vector<vector<MaxHeapElement<int> > >& knn) {
        // ivecs file
        ofstream fout(ivecsBenchFileName, ios::binary);
        if (!fout) {
            cout << "cannot create output file " << ivecsBenchFileName << endl;
            assert(false);
        }
        int K = knn[0].size();
        for (int i = 0; i < knn.size(); ++i) {
            assert(knn[i].size() == K);
            fout.write((char*)&K, sizeof(int));
            const vector<MaxHeapElement<int>> topker = knn[i];
            for (int idx = 0; idx < topker.size(); ++idx) {
                fout.write((char*)&topker[idx].data(), sizeof(int));
            }
        }
        fout.close();
        cout << "ivecs groundtruth are written into " << ivecsBenchFileName << endl;
    }
};

int main(int argc, char** argv) {

    typedef float DataType;

    if (argc != 7 && argc != 8) {
        cout << "usage: program base_file.fvecs query_file.fvecs K groundtruth_file.lshbox groundtruth_file.ivecs num_threads=4" << endl;
        return 0;
    }

    const char* baseFileName = argv[1];
    const char* queryFileName = argv[2];
    int K = std::atoi(argv[3]);
    const char* lshboxBenchFileName = argv[4];
    const char* ivecsBenchFileName = argv[5];
    string metric = argv[6];
    int numThreads = 4;
    if (argc >= 8)
        numThreads = stoi(argv[7]);

    lshbox::Matrix<DataType> base_data (baseFileName);
    lshbox::Matrix<DataType> query_data(queryFileName);

    vector<vector<MaxHeapElement<int> > > knn = ss::ExactKNN<DataType >(
            query_data[0],
            query_data.getSize(),
            base_data[0],
            base_data.getSize(),
            query_data.getDim(),
            K,
            ss::EuclidDistance<DataType >);

    GroundWriter writer;
    writer.writeLSHBOX<DataType>(lshboxBenchFileName, knn);
    writer.writeIVECS<DataType>(ivecsBenchFileName, knn);
    return 0;
}
