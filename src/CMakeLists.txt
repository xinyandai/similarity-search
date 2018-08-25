cmake_minimum_required(VERSION 2.8)

project(SimilaritySearch)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package( PkgConfig )
pkg_check_modules( EIGEN3 REQUIRED eigen3 )
include_directories( ${EIGEN3_INCLUDE_DIRS} )

#find_package (Eigen3 REQUIRED)
#include_directories(${EIGEN3_INCLUDE_DIR})

find_package(Boost COMPONENTS system filesystem regex thread program_options date_time chrono)
include_directories(${Boost_INCLUDE_DIRS})


find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

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
        kmeans
        pq
        graph
        ground_truth
)

FOREACH(EXAMPLE ${EXAMPLES} )
    ADD_EXECUTABLE(${EXAMPLE} ${EXAMPLE}.cpp)
    target_link_libraries(${EXAMPLE} ${Boost_LIBRARIES})
ENDFOREACH(EXAMPLE)

