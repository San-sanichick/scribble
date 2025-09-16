cmake_minimum_required(VERSION 4.0)
project(scribble)

if (POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
endif(POLICY CMP0072)


# add_definitions(-std=c++20)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/target/$<CONFIG>")

file(GLOB_RECURSE src src/*.cpp src/**/*.cpp)

add_executable(scribble ${src})

# define DEBUG_BUILD macro
add_compile_definitions($<$<CONFIG:Debug>:DEBUG_BUILD>)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PORTAL REQUIRED libportal)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(GIO REQUIRED gio-2.0)

target_include_directories(scribble PUBLIC
    include/
    ${PORTAL_INCLUDE_DIRS}
    ${GLIB_INCLUDE_DIRS}
    ${GIO_INCLUDE_DIRS}
)

add_subdirectory(vendors/raylib)


target_link_libraries(scribble PUBLIC
    raylib
    ${PORTAL_LIBRARIES}
    ${GLIB_LIBRARIES}
    ${GIO_LIBRARIES}
)

set(PCH ${CMAKE_SOURCE_DIR}/include/pch.hpp)

target_precompile_headers(scribble PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${PCH}>)

target_compile_options(scribble PRIVATE
    -ffile-prefix-map=${CMAKE_SOURCE_DIR}=.
    -Wall
)


target_link_options(scribble PRIVATE
    -fno-exceptions
    -fanalyzer

    $<$<CONFIG:Release>:-O3 -g0>
    $<$<CONFIG:Debug>:-Og -g3>
)
