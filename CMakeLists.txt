cmake_minimum_required(VERSION 3.10)
project(zeta)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig)
pkg_search_module(FLINT REQUIRED flint)

add_executable(zeta
    main.cpp
    src/reader.cpp
    src/acb_vector.cpp
    src/solver.cpp
)

target_include_directories(zeta PRIVATE ${FLINT_INCLUDE_DIRS})
target_compile_options(zeta PRIVATE ${FLINT_CFLAGS})
target_link_libraries(zeta PRIVATE ${FLINT_LIBRARIES})
