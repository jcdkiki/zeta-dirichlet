cmake_minimum_required(VERSION 3.27)
project(zeta)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(flint-wrappers)
add_subdirectory(zeta)
add_subdirectory(zeta-gui)
