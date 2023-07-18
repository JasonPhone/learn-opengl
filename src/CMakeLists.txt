set(HEADER_DIR "${CMAKE_SOURCE_DIR}/include")

add_library(Shader
  STATIC
  # SHARED
  ${HEADER_DIR}/learn-opengl/Shader.h
  ./learn-opengl/Shader.cpp
  # ./glad.c
)

add_library(Camera
  STATIC
  ${HEADER_DIR}/learn-opengl/Camera.h
  ./learn-opengl/Camera.cpp
)

add_library(Model
  STATIC
  ${HEADER_DIR}/learn-opengl/Model.h
  ./learn-opengl/Model.cpp
)
# target_link_libraries(model PRIVATE assimp::assimp)