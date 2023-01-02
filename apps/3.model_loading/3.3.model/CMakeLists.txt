cmake_minimum_required(VERSION 3.5.0)

project("3.3.model")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/)

add_executable(${PROJECT_NAME}
  ${CMAKE_SOURCE_DIR}/src/glad.c
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  "${CMAKE_SOURCE_DIR}/lib/libglfw3.a"
  shader
  camera
  stb_image
  assimp::assimp
  model
)

# Copy assimp dll into this folder
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  "${CMAKE_SOURCE_DIR}/bin/"
  $<TARGET_FILE_DIR:${PROJECT_NAME}>
)