add_library(acb_wrappers
    src/matrix.cpp
    src/vector.cpp
)

find_package(PkgConfig)
pkg_search_module(FLINT REQUIRED flint)

target_include_directories(acb_wrappers PUBLIC include)
target_link_libraries(acb_wrappers PUBLIC flint)
