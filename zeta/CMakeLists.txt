add_library(zeta
    src/dirichlet_series.cpp
    src/nested_systems_solver.cpp
    src/reader.cpp
)

target_include_directories(zeta PUBLIC include)
target_link_libraries(zeta PUBLIC flint_wrappers)
