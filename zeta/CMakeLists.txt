add_library(zeta
    src/dirichlet_series.cpp
    src/nested_systems_solver.cpp
    src/reader_solutions.cpp
    src/reader.cpp
    src/solver.cpp
)

target_include_directories(zeta PUBLIC include)
target_link_libraries(zeta PUBLIC acb_wrappers)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    target_compile_options(zeta PRIVATE -fsanitize=address)
    target_link_options(zeta PRIVATE -fsanitize=address)
endif()
