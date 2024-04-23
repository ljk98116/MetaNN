cmake_minimum_required(VERSION 3.22)
project("MetaNN Test")

include_directories(METANN PRIVATE ${CMAKE_SOURCE_DIR})
add_subdirectory(METANN)
add_subdirectory(test)
