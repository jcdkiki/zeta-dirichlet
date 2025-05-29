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

if(GMP_NEEDS_BUILD)
    add_dependencies(flint_wrappers gmp)
endif()
if(MPFR_NEEDS_BUILD)
    add_dependencies(flint_wrappers mpfr)
endif()
if(FLINT_NEEDS_BUILD)
    add_dependencies(flint_wrappers flint)
endif()

target_include_directories(flint_wrappers PUBLIC include ${FLINT_INCLUDE_DIR})
target_link_libraries(flint_wrappers PUBLIC ${GMP_LIBRARY} ${MPFR_LIBRARY} ${FLINT_LIBRARY})

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_compile_definitions(DEBUG)
endif()
