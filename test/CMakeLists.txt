cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 20)
project("MetaNN Test")

find_package(CppUTest REQUIRED)
# set(CPPUTEST_HOME "/usr/local/include/CppUTest")

find_library(CppUTest, "/usr/local/include/CppUTest/")
find_library(CppUTestExt, "/usr/local/include/CppUTest/")

add_executable(example_test ExampleTest.cpp)

target_link_libraries(example_test PRIVATE
    CppUTest
    CppUTestExt)

add_executable(varTypeDictTest VarTypeDictTest.cpp)

target_link_libraries(varTypeDictTest PRIVATE
    CppUTest
    CppUTestExt)

add_executable(sfinaeTest SFINAE.cpp)