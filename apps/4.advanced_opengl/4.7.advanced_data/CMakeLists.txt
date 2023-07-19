cmake_minimum_required(VERSION 3.5.0)

project("4.7.advanced_data")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/)

add_executable(${PROJECT_NAME}
  ${CMAKE_SOURCE_DIR}/src/glad.c
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_link_libraries(${PROJECT_NAME}
  "${CMAKE_SOURCE_DIR}/lib/libglfw3.a"
  Shader
  Camera
  image
)