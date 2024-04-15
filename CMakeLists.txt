cmake_minimum_required(VERSION 3.7)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(MyProject VERSION 1.0
                  DESCRIPTION "Cilk"
                  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-stack-protector")

find_package(benchmark REQUIRED)

add_executable(cilk src/benchmark.cpp src/simple_scheduler.hpp
    src/simple_cs_scheduler.hpp src/scheduler.hpp)

add_executable(test src/cont_test.cpp src/simple_cont_scheduler.hpp)

target_link_libraries(cilk benchmark::benchmark)