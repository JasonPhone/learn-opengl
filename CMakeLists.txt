cmake_minimum_required(VERSION 3.5.0)

project(gl_learn)

# find_package(OpenGL REQUIRED)
include_directories(
  ./extern
  ./include
  ./src
)

# add_library(hello
# STATIC
# ./src/hello.h
# ./src/hello.cpp
# )

# add_executable(gl_learn ./src/glad.c ./src/main.cpp)

# target_link_libraries(gl_learn
# "${PROJECT_SOURCE_DIR}/lib/libglfw3.a"
# )
add_subdirectory(./apps/1.5.shader)
add_subdirectory(./src)
