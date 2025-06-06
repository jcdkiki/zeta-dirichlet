cmake_minimum_required(VERSION 3.27)
project(zeta)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++")
    set(MINGW_PREFIX ${PROJECT_SOURCE_DIR}/mingw64)
    set(CMAKE_PREFIX_PATH ${MINGW_PREFIX} ${CMAKE_PREFIX_PATH})

    include(ExternalProject)
    include(FetchContent)

    ExternalProject_Add(
        gmp
        URL https://ftp.gnu.org/gnu/gmp/gmp-6.3.0.tar.xz
        PREFIX ${CMAKE_BINARY_DIR}/gmp
        CONFIGURE_COMMAND <SOURCE_DIR>/configure --host=${TOOLCHAIN_PREFIX} --prefix=${MINGW_PREFIX} --enable-static --disable-shared
        BUILD_COMMAND make
        INSTALL_COMMAND make install
    )
    set(GMP_LIBRARY ${MINGW_PREFIX}/lib/libgmp.a)
    set(GMP_INCLUDE_DIR ${MINGW_PREFIX}/include)

    ExternalProject_Add(
        mpfr
        URL https://www.mpfr.org/mpfr-current/mpfr-4.2.2.tar.xz
        PREFIX ${CMAKE_BINARY_DIR}/mpfr
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        CONFIGURE_COMMAND <SOURCE_DIR>/configure --host=${TOOLCHAIN_PREFIX} --prefix=${MINGW_PREFIX} --with-gmp=${MINGW_PREFIX} --enable-static=yes --enable-shared=no
        BUILD_COMMAND make
        INSTALL_COMMAND make install
        DEPENDS gmp
    )
    set(MPFR_LIBRARY ${MINGW_PREFIX}/lib/libmpfr.a)
    set(MPFR_INCLUDE_DIR ${MINGW_PREFIX}/include)

    ExternalProject_Add(
        flint
        URL https://github.com/flintlib/flint/releases/download/v3.2.2/flint-3.2.2.tar.xz
        PREFIX ${CMAKE_BINARY_DIR}/flint
        CONFIGURE_COMMAND <SOURCE_DIR>/configure --host=${TOOLCHAIN_PREFIX} --prefix=${MINGW_PREFIX} --with-gmp=${MINGW_PREFIX} --with-mpfr=${MINGW_PREFIX} --enable-static --disable-shared
        INSTALL_COMMAND make install
        DEPENDS gmp mpfr
    )
    set(FLINT_LIBRARIES ${MINGW_PREFIX}/lib/libflint.a ${MPFR_LIBRARY} ${GMP_LIBRARY})
    set(FLINT_INCLUDE_DIRS ${MINGW_PREFIX}/include/flint)

    set(FETCHCONTENT_QUIET FALSE)
    FetchContent_Declare(
        wxWidgets_external
        URL "https://github.com/wxWidgets/wxWidgets/releases/download/v3.2.8/wxWidgets-3.2.8.7z"
    )
    set(wxBUILD_SHARED OFF)
    FetchContent_MakeAvailable(wxWidgets_external)
    set(wxWidgets_LIBRARIES wxcore wxgl)
else()
    find_package(wxWidgets COMPONENTS core base gl REQUIRED)
    include(${wxWidgets_USE_FILE})
    find_package(PkgConfig)
    pkg_search_module(FLINT REQUIRED flint)
endif()

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

add_subdirectory(flint-wrappers)
add_subdirectory(zeta)
add_subdirectory(zeta-gui)
