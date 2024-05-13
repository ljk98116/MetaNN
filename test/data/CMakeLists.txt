cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 20)
project("MetaNN Test")

add_library(test_array SHARED test_array.cpp)
