cmake_minimum_required(VERSION 3.5.0)

project(learn-opengl)

# find_package(OpenGL REQUIRED)
include_directories(
  ./extern
  ./include
  ./src
)

add_subdirectory(./apps)

add_subdirectory(./src)
