cmake_minimum_required(VERSION 3.27)
project(zeta)

include(ExternalProject)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++")
    set(MINGW_PREFIX ${PROJECT_SOURCE_DIR}/mingw64)
    set(CMAKE_PREFIX_PATH ${MINGW_PREFIX} ${CMAKE_PREFIX_PATH})
    set(WX_CONFIGURE_FLAGS --with-msw --disable-shared)
    set(GMP_CONFIGURE_FLAGS --enable-static --disable-shared)
    set(MPFR_CONFIGURE_FLAGS --enable-static=yes --enable-shared=no)
    set(FLINT_CONFIGURE_FLAGS --enable-static --disable-shared)
endif()


# GMP stuff
find_library(GMP_LIBRARY NAMES gmp libgmp PATHS ${MINGW_PREFIX}/lib NO_DEFAULT_PATH)
find_path(GMP_INCLUDE_DIR gmp.h PATHS ${MINGW_PREFIX}/include NO_DEFAULT_PATH)

if(NOT GMP_LIBRARY OR NOT GMP_INCLUDE_DIR)
    ExternalProject_Add(
        gmp
        URL https://ftp.gnu.org/gnu/gmp/gmp-6.3.0.tar.xz
        PREFIX ${CMAKE_BINARY_DIR}/gmp
        CONFIGURE_COMMAND <SOURCE_DIR>/configure --host=${TOOLCHAIN_PREFIX} --prefix=${MINGW_PREFIX} ${GMP_CONFIGURE_FLAGS}
        BUILD_COMMAND make
        INSTALL_COMMAND make install
    )
    set(GMP_LIBRARY ${MINGW_PREFIX}/lib/libgmp.a)
    set(GMP_INCLUDE_DIR ${MINGW_PREFIX}/include)
    set(GMP_NEEDS_BUILD TRUE)
else()
    set(GMP_NEEDS_BUILD FALSE)
endif()


# MPFR stuff
find_library(MPFR_LIBRARY NAMES mpfr libmpfr PATHS ${MINGW_PREFIX}/lib NO_DEFAULT_PATH)
find_path(MPFR_INCLUDE_DIR mpfr.h PATHS ${MINGW_PREFIX}/include NO_DEFAULT_PATH)

if(NOT MPFR_LIBRARY OR NOT MPFR_INCLUDE_DIR)
    ExternalProject_Add(
        mpfr
        URL https://www.mpfr.org/mpfr-current/mpfr-4.2.2.tar.xz
        PREFIX ${CMAKE_BINARY_DIR}/mpfr
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        CONFIGURE_COMMAND <SOURCE_DIR>/configure --host=${TOOLCHAIN_PREFIX} --prefix=${MINGW_PREFIX} --with-gmp=${MINGW_PREFIX} ${MPFR_CONFIGURE_FLAGS}
        BUILD_COMMAND make
        INSTALL_COMMAND make install
        DEPENDS gmp
    )
    set(MPFR_LIBRARY ${MINGW_PREFIX}/lib/libmpfr.a)
    set(MPFR_INCLUDE_DIR ${MINGW_PREFIX}/include)
    set(MPFR_NEEDS_BUILD TRUE)
else()
    set(MPFR_NEEDS_BUILD FALSE)
endif()


# Flint stuff
find_library(FLINT_LIBRARY NAMES flint libflint PATHS ${MINGW_PREFIX}/lib NO_DEFAULT_PATH)
find_path(FLINT_INCLUDE_DIR flint/flint.h PATHS ${MINGW_PREFIX}/include NO_DEFAULT_PATH)

if(NOT FLINT_LIBRARY OR NOT FLINT_INCLUDE_DIR)
    ExternalProject_Add(
        flint
        URL https://github.com/flintlib/flint/releases/download/v3.2.2/flint-3.2.2.tar.xz
        PREFIX ${CMAKE_BINARY_DIR}/flint
        CONFIGURE_COMMAND <SOURCE_DIR>/configure --host=${TOOLCHAIN_PREFIX} --prefix=${MINGW_PREFIX} --with-gmp=${MINGW_PREFIX} --with-mpfr=${MINGW_PREFIX} ${FLINT_CONFIGURE_FLAGS}
        BUILD_COMMAND make
        INSTALL_COMMAND make install
        DEPENDS gmp mpfr
    )
    set(FLINT_LIBRARY ${MINGW_PREFIX}/lib/libflint.a)
    set(FLINT_INCLUDE_DIR ${MINGW_PREFIX}/include/flint)
    set(FLINT_NEEDS_BUILD TRUE)
else()
    set(FLINT_NEEDS_BUILD FALSE)
endif()


# wxWidgets stuff
find_package(wxWidgets COMPONENTS core base PATHS ${MINGW_PREFIX}/lib/cmake NO_DEFAULT_PATH)

if(NOT wxWidgets_FOUND)
    ExternalProject_Add(
        wxwidgets
        URL https://github.com/wxWidgets/wxWidgets/releases/download/v3.2.4/wxWidgets-3.2.4.tar.bz2
        PREFIX ${CMAKE_BINARY_DIR}/wxwidgets
        CONFIGURE_COMMAND <SOURCE_DIR>/configure --host=${TOOLCHAIN_PREFIX} --prefix=${MINGW_PREFIX} --enable-unicode --with-opengl ${WX_CONFIGURE_FLAGS}
        BUILD_COMMAND make -j 16
        INSTALL_COMMAND make install
    )
    set(wxWidgets_ROOT_DIR ${MINGW_PREFIX})
    set(WXWIDGETS_NEEDS_BUILD TRUE)
else()
    set(WXWIDGETS_NEEDS_BUILD FALSE)
endif()


# OpenGL stuff
find_package(OpenGL REQUIRED)

add_subdirectory(flint-wrappers)
add_subdirectory(zeta)
add_subdirectory(zeta-gui)
