cmake_minimum_required(VERSION 4.0)
project(scribble)

if (POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
endif(POLICY CMP0072)


add_definitions(-std=c++20)
set(CMAKE_CXX_STANDARD 20)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/target/$<CONFIG>")

file(GLOB_RECURSE src src/*.cpp src/**/*.cpp)

add_executable(scribble ${src} "lib/glad/src/glad.c" ${res})


# define DEBUG_BUILD macro
add_compile_definitions($<$<CONFIG:Debug>:DEBUG_BUILD>)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

target_include_directories(scribble PUBLIC
    include/
    ${OPENGL_INCLUDE_DIR}
    lib/glad/include/
)

add_subdirectory(vendors/glm)


target_link_libraries(scribble PUBLIC
    ${OPENGL_gl_LIBRARY}
    ${GLFW_LIBRARIES}
    glfw
    glm
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

    # -lglfw3
    # -lGL
    # -lX11
    # -lpthread
    # -lxrandr
    # -lXi
    # -ldl

    $<$<CONFIG:Release>:-O3 -g0>
    $<$<CONFIG:Debug>:-Og -g3>
)
