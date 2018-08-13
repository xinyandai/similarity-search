cmake_minimum_required(VERSION 2.8)

project(SimilaritySearch)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(Boost COMPONENTS system filesystem regex thread program_options date_time chrono)
if(NOT Boost_FOUND)
	message("NOT FOUND BOOST")
endif()

include_directories(${Boost_INCLUDE_DIRS})

link_libraries(-lpthread)

INCLUDE_DIRECTORIES(
	${SimilaritySearch_SOURCE_DIR}/include
)

SET(EXAMPLES 
    search
    normrange
)

FOREACH(EXAMPLE ${EXAMPLES} )
	ADD_EXECUTABLE(${EXAMPLE} ${EXAMPLE}.cpp)
	target_link_libraries(${EXAMPLE} ${Boost_LIBRARIES})
ENDFOREACH(EXAMPLE)

