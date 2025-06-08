if(WIN32)
    set(RC_FILE "src/resources.rc")
else()
    set(RC_FILE "")
endif()

add_executable(zeta-gui
    src/main.cpp
    src/plot_panel.cpp
    src/zeta_app.cpp
    src/zeta_frames.cpp
    
    src/zeta_frame_handlers.cpp
    src/rendering_settings_frame_handlers.cpp
    src/new_file_frame_handlers.cpp

    src/shared.cpp
    ${RC_FILE}
)

target_include_directories(zeta-gui PRIVATE include ${wxWidgets_INCLUDE_DIRS})
target_link_libraries(zeta-gui PRIVATE ${wxWidgets_LIBRARIES} OpenGL::GL zeta)

if (WIN32)
    target_compile_definitions(zeta-gui PRIVATE ZETA_WIN32)
    target_link_options(zeta-gui PRIVATE -mwindows)
    target_link_libraries(zeta-gui PRIVATE -lpthread -static)
endif()

add_custom_target(pack_7z
    COMMAND ${CMAKE_COMMAND} -E tar "cfv" "${CMAKE_BINARY_DIR}/zeta-gui.7z" --format=7zip "$<TARGET_FILE:zeta-gui>"
    DEPENDS zeta-gui
    COMMENT "Creating 7z archive of zeta-gui"
)
