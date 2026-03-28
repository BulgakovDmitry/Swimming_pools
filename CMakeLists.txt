cmake_minimum_required(VERSION 3.16)

project(Swimming_pools LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(swimming_pools_headers INTERFACE)
target_include_directories(swimming_pools_headers
    INTERFACE ${CMAKE_SOURCE_DIR}/include
)
add_library(swimming_pools::headers ALIAS swimming_pools_headers)

add_executable(Swimming_pools
    src/main.cpp
)

target_include_directories(Swimming_pools PRIVATE include)

enable_testing()
add_subdirectory(tests)