cmake_minimum_required(VERSION 3.10)
project(zeta)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig)
pkg_search_module(FLINT REQUIRED flint)

find_package(wxWidgets REQUIRED)
include(${wxWidgets_USE_FILE})

file(GLOB_RECURSE ZETA_SRC "src/*.cpp")
add_executable(zeta ${ZETA_SRC})

target_include_directories(zeta PRIVATE ${FLINT_INCLUDE_DIRS} ${wxWidgets_INCLUDE_DIRS})
target_compile_options(zeta PRIVATE ${FLINT_CFLAGS} ${wxWidgets_CXX_FLAGS})
target_link_libraries(zeta PRIVATE ${FLINT_LIBRARIES} ${wxWidgets_LIBRARIES})

if(CMAKE_BUILD_TYPE MATCHES "Debug" OR CMAKE_BUILD_TYPE MATCHES "Release")
    target_compile_options(zeta PRIVATE -fsanitize=address)
    target_link_options(zeta PRIVATE -fsanitize=address)
endif()
