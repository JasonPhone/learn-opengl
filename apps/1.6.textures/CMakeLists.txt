cmake_minimum_required(VERSION 3.5.0)

project("1.6.textures")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(${PROJECT_NAME}
  ${CMAKE_SOURCE_DIR}/src/glad.c
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  # ${CMAKE_CURRENT_SOURCE_DIR}/stb.cpp
)

target_link_libraries(${PROJECT_NAME}
  "${CMAKE_SOURCE_DIR}/lib/libglfw3.a"
  shader
  stb_image
)
