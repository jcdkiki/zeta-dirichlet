add_library(flint_wrappers
    src/matrix.cpp
    src/vector.cpp

    # are there really needed?
    include/flint_wrappers/common.hpp
    include/flint_wrappers/complex.hpp
    include/flint_wrappers/matrix.hpp
    include/flint_wrappers/vector.hpp
)

if(WIN32)
    add_dependencies(flint_wrappers gmp)
    add_dependencies(flint_wrappers mpfr)
    add_dependencies(flint_wrappers flint)
endif()

target_include_directories(flint_wrappers PUBLIC include ${FLINT_INCLUDE_DIRS})
target_link_libraries(flint_wrappers PUBLIC ${FLINT_LIBRARIES})

if(WIN32)
    target_link_libraries(flint_wrappers PUBLIC -lpthread -static)
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_compile_definitions(DEBUG)
endif()
