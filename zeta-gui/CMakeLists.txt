set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

find_package(wxWidgets COMPONENTS core base gl REQUIRED)
include(${wxWidgets_USE_FILE})

add_executable(zeta-gui
    src/main.cpp
    src/plot_panel.cpp
    src/zeta_app.cpp
    src/zeta_frame.cpp
)

target_include_directories(zeta-gui PRIVATE include)
target_link_libraries(zeta-gui PRIVATE GL zeta wxWidgets::wxWidgets)
