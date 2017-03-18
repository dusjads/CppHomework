cmake_minimum_required(VERSION 2.8)

project(PERSET)

include_directories(${PERSET_SOURCE_DIR})

add_executable(per_set_testing
				per_set_tests_random.cpp
               #per_set_testing.cpp
               	per_set.h
               	per_set.cpp)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -std=c++11 -pedantic")
endif()

target_link_libraries(per_set_testing -O2)