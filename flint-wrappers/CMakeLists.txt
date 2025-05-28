add_library(flint_wrappers
    src/matrix.cpp
    src/vector.cpp

    # are there really needed?
    include/flint_wrappers/common.hpp
    include/flint_wrappers/complex.hpp
    include/flint_wrappers/matrix.hpp
    include/flint_wrappers/vector.hpp
)

find_package(PkgConfig)
pkg_search_module(FLINT REQUIRED flint)

target_include_directories(flint_wrappers PUBLIC include)
target_link_libraries(flint_wrappers PUBLIC flint)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_compile_definitions(DEBUG)
endif()
