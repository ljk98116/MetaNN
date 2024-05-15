cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 17)
project("MetaNN Test")

add_definitions("-Wall -g")
add_library(test_array SHARED test_array.cpp)
add_library(test_batch_matrix SHARED test_batch_matrix.cpp)

