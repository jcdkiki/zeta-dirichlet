add_executable(zeta-cli
    src/main.cpp
)

target_include_directories(zeta-cli PRIVATE include argparse)
target_link_libraries(zeta-gui PRIVATE zeta)
