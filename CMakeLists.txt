cmake_minimum_required(VERSION 3.10)
project(zeta)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig)
pkg_search_module(FLINT REQUIRED flint)

set(MAIN_SOURCES "main.cpp")
file(GLOB_RECURSE SRC_SOURCES "src/*.cpp")

add_executable(zeta
    ${MAIN_SOURCES}
    ${SRC_SOURCES}
)

target_include_directories(zeta PRIVATE ${FLINT_INCLUDE_DIRS})
target_compile_options(zeta PRIVATE ${FLINT_CFLAGS})
target_link_libraries(zeta PRIVATE ${FLINT_LIBRARIES})

if(CMAKE_BUILD_TYPE MATCHES "Release")
    target_compile_options(zeta PRIVATE -fsanitize=address)
    target_link_options(zeta PRIVATE -fsanitize=address)
endif()
