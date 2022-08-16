cmake_minimum_required(VERSION 3.5.0)

project(gl_learn)

# find_package(OpenGL REQUIRED)
include_directories(
  ./include
)

# add_library(hello
#   STATIC
#   ./src/hello.h
#   ./src/hello.cpp
# )

add_executable(gl_learn ./src/glad.c ./src/main.cpp)

target_link_libraries(gl_learn
  "${PROJECT_SOURCE_DIR}/lib/libglfw3.a"
)
