ITQ
===
# run

    mkdir build
    cd build && cmake .. && make itq
    ./itq -t ../../data/audio/audio_base.fvecs -b ../../data/audio/audio_base.fvecs -q ../../data/audio/audio_query.fvecs -g ../../data/audio/20_audio_euclid_groundtruth.lshbox -i 10
# modules
1. entrance
    - file: [similarity-search/src/apps/itq.cpp](https://github.com/xinyandai/similarity-search/blob/master/src/apps/itq.cpp)

2. loading data
    - file: [similarity-search/src/include/executor.hpp](https://github.com/xinyandai/similarity-search/blob/master/src/include/executor.hpp)

3. training
    - file: [similarity-search/src/include/index/itq.hpp](https://github.com/xinyandai/similarity-search/blob/master/src/include/index/itq.hpp)
    - function: void Train(const lshbox::Matrix<DataType> &)

4. Build Index
    - file: [similarity-search/src/include/index/itq.hpp](https://github.com/xinyandai/similarity-search/blob/master/src/include/index/itq.hpp)
    - file: [similarity-search/src/include/index/map_index.hpp](https://github.com/xinyandai/similarity-search/blob/master/src/include/index/map_index.hpp)
    - function: void Add(const lshbox::Matrix<DataType> &)

5. Query
    - file: [similarity-search/src/include/query/hamming_ranker.hpp](https://github.com/xinyandai/similarity-search/blob/master/src/include/query/hamming_ranker.hpp)