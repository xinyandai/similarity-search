cmake_minimum_required(VERSION 2.8)

project(SimilaritySearch)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(OpenMP)
if (OPENMP_FOUND)
    message("-- [Found PKG] Found OpenMP")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

include_directories(
        ${SimilaritySearch_SOURCE_DIR}/include
)

if ("${MY_EIGEN}" STREQUAL "")
    message("-- [Found PKG] Found EIGEN with find_package")
    find_package (Eigen3 REQUIRED)
    include_directories("${EIGEN3_INCLUDE_DIR}")
    MESSAGE("-- [Found PKG] eigen3 in ${EIGEN3_INCLUDE_DIR}")
else()
    message("-- [Warning] Found EIGEN with hardcode path: ${MY_EIGEN}")
    include_directories("${MY_EIGEN}")
endif ()

if ("${MY_BOOST}" STREQUAL "")
    message("-- [Found PKG] Found boost with find_package")
    find_package(Boost COMPONENTS system filesystem regex thread program_options date_time chrono)
    include_directories(${Boost_INCLUDE_DIRS})
    link_libraries(${Boost_LIBRARIES})
    MESSAGE("-- [Found PKG] boost in ${Boost_INCLUDE_DIRS}")
else()
    message("-- [Warning] Found boost with hardcode path: ${MY_BOOST}")
    include_directories(/research/jcheng2/xinyan/env/boost_install/include)
    link_libraries(
            ${MY_BOOST}/libboost_program_options.so
            ${MY_BOOST}/libboost_system.so
    )
endif ()

SET(EXAMPLES
        srp
        cross_polytope
        e2lsh
        itq
        kmeans
        pq
        graph
        rpt
        forest
        joint_inverted
        multi_pq)

FOREACH(EXAMPLE ${EXAMPLES} )
    ADD_EXECUTABLE(${EXAMPLE} apps/${EXAMPLE}.cpp)
ENDFOREACH(EXAMPLE)

SET(MIPS
        rational_nr
        rational_sign
        rational_alsh
        norm_range
        float_lsh
        simple_lsh
        simple_e2lsh
        cross_polytope_simple_lsh
        rational_cross_polytope
        sign_alsh
        alsh)

FOREACH(EXAMPLE ${MIPS} )
    ADD_EXECUTABLE(${EXAMPLE} mips/${EXAMPLE}.cpp)
ENDFOREACH(EXAMPLE)


SET(EXAMPLES
        ground_truth)

FOREACH(EXAMPLE ${EXAMPLES} )
    ADD_EXECUTABLE(${EXAMPLE} tool/${EXAMPLE}.cpp)
ENDFOREACH(EXAMPLE)