cmake_minimum_required(VERSION 3.20)

project(AlgorithmsC VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)
enable_testing()

option(ENABLE_COVERAGE "Enable code coverage instrumentation" OFF)

add_subdirectory(Algorithms_C)
