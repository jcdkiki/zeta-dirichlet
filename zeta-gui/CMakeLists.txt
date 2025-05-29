set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

find_package(wxWidgets COMPONENTS core base gl REQUIRED)

if(${wxWidgets_USE_FILE})
    include(${wxWidgets_USE_FILE})
endif()

if(WIN32)
    set(RC_FILE "src/resources.rc")
else()
    set(RC_FILE "")
endif()

add_executable(zeta-gui
    src/main.cpp
    src/plot_panel.cpp
    src/zeta_app.cpp
    src/zeta_frame.cpp
    ${RC_FILE}
)

if(WXWIDGETS_NEEDS_BUILD)
    add_dependencies(zeta-gui wxwidgets)
endif()

target_include_directories(zeta-gui PRIVATE include)
target_link_libraries(zeta-gui PRIVATE wxWidgets::wxWidgets ${MPFR_LIBRARY} ${FLINT_LIBRARY} ${GMP_LIBRARY} OpenGL::GL zeta)

if (WIN32)
    target_link_libraries(zeta-gui PRIVATE -lpthread -static)
endif()
