cmake_minimum_required(VERSION 2.8)

project(SimilaritySearch)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(Boost COMPONENTS system filesystem regex thread program_options date_time chrono)
find_package (Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIRS})

link_libraries(-lpthread)

INCLUDE_DIRECTORIES(
    ${SimilaritySearch_SOURCE_DIR}/include
)

SET(EXAMPLES
        srp
        e2lsh
        itq
        norm_range
        simple_lsh
)

FOREACH(EXAMPLE ${EXAMPLES} )
    ADD_EXECUTABLE(${EXAMPLE} ${EXAMPLE}.cpp)
    target_link_libraries(${EXAMPLE} ${Boost_LIBRARIES})
ENDFOREACH(EXAMPLE)

