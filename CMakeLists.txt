cmake_minimum_required(VERSION 3.10)
project(zeta)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Путь к FLINT (если он в include проекта)
include_directories("/usr/include/flint")  # или точный путь к flint

# Добавляем исходные файлы из папки src
add_executable(zeta
    main.cpp
    src/reader.cpp
    src/acb_vector.cpp
    src/solver.cpp
)

target_link_libraries(zeta
    PRIVATE
    flint
    gmp  
    mpfr
)