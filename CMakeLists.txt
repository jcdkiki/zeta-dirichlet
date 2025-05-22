cmake_minimum_required(VERSION 3.10)
project(zeta)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig)
pkg_search_module(FLINT REQUIRED flint)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

find_package(wxWidgets COMPONENTS core base gl REQUIRED)
include(${wxWidgets_USE_FILE})

add_executable(zeta 
    src/acb_matrix.cpp
    src/acb_vector.cpp
    src/dirichlet_series.cpp
    src/main.cpp
    src/plot_panel.cpp
    src/reader_solutions.cpp
    src/reader.cpp
    src/solver.cpp
    src/zeta_app.cpp
    src/zeta_frame.cpp
    src/nested_systems_solver.cpp
)

target_include_directories(zeta PRIVATE include)

target_include_directories(zeta PRIVATE ${FLINT_INCLUDE_DIRS} ${wxWidgets_INCLUDE_DIRS})
target_compile_options(zeta PRIVATE ${FLINT_CFLAGS} ${wxWidgets_CXX_FLAGS})
target_link_libraries(zeta PRIVATE ${FLINT_LIBRARIES} ${wxWidgets_LIBRARIES} GL)

if(CMAKE_BUILD_TYPE MATCHES "Release")
    target_compile_options(zeta PRIVATE -fsanitize=address)
    target_link_options(zeta PRIVATE -fsanitize=address)
endif()
