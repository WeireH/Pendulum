cmake_minimum_required(VERSION 3.20)

project(
    Pendulum
    VERSION 1.0
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)

add_subdirectory(external/glfw/)

find_package(glm REQUIRED)

add_executable(
    Pendulum
    src/main.cpp
    external/glad/src/glad.c
)

target_include_directories(
    Pendulum PRIVATE
    external/glad/include
)

target_link_libraries(
    Pendulum PRIVATE
    glfw
    OpenGL::GL
    glm::glm
)