cmake_minimum_required(VERSION 3.3)

project(demo VERSION 0.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(main)
add_subdirectory(debug)
