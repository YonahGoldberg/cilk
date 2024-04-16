cmake_minimum_required(VERSION 3.7)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(MyProject VERSION 1.0
                  DESCRIPTION "Cilk"
                  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(benchmark REQUIRED)

add_executable(cilk src/benchmark.cpp src/simple_scheduler.hpp
    src/simple_cs_scheduler.hpp src/scheduler.hpp src/tests/fib.hpp src/tests/fib.cpp src/tests/quicksort.hpp
    src/tests/quicksort.cpp src/tests/quicksort.hpp src/tests/fib.cpp src/tests/fib.hpp src/tests/scheduler_instance.hpp
    src/tests/rectmul.cpp src/tests/rectmul.hpp src/tests/nqueens.cpp src/tests/nqueens.hpp)

target_link_libraries(cilk benchmark::benchmark)