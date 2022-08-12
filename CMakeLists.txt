cmake_minimum_required(VERSION 3.0.0)                  

project(gl_learn)                                         

# find_package(OpenGL REQUIRED)

include_directories(
    ./include
)                           

link_libraries(
    "${PROJECT_SOURCE_DIR}/lib/libglfw3.a"
)

add_executable(gl_learn ./src/glad.c ./src/main.cpp)
