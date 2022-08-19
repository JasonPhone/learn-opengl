cmake_minimum_required(VERSION 3.5.0)

project("1.5.shader")

add_executable(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/src/glad.c ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_link_libraries(${PROJECT_NAME}
  "${CMAKE_SOURCE_DIR}/lib/libglfw3.a"
  shader
)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR})