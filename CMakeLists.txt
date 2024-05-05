cmake_minimum_required(VERSION 3.7)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(MyProject VERSION 1.0
                  DESCRIPTION "Cilk"
                  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(benchmark REQUIRED)

add_executable(cilk src/benchmark.cpp src/schedulers/simple_scheduler.hpp src/schedulers/no_spawn_scheduler.hpp
    src/schedulers/child_scheduler.hpp src/schedulers/scheduler.hpp src/tests/fib.hpp src/tests/fib.cpp src/tests/quicksort.hpp
    src/tests/quicksort.cpp src/tests/quicksort.hpp src/tests/fib.cpp src/tests/fib.hpp src/scheduler_instance.hpp
    src/tests/rectmul.cpp src/tests/rectmul.hpp src/tests/nqueens.cpp src/tests/nqueens.hpp src/tests/nbody.cpp src/tests/nbody.hpp 
    src/tests/heat.cpp src/tests/heat.hpp src/scheduler_instance.cpp src/tests/pfor.hpp
    src/tests/pfor.cpp)

target_link_libraries(cilk benchmark::benchmark)