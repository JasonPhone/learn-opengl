set(HEADER_DIR "${CMAKE_SOURCE_DIR}/include")


add_library(shader
  STATIC
  ${HEADER_DIR}/learn-opengl/shader.h
  ./learn-opengl/shader.cpp
)

add_library(stb_image
  STATIC
  ${HEADER_DIR}/stb_image.h
  stb_image.cpp
)

add_library(camera
  STATIC
  ${HEADER_DIR}/learn-opengl/camera.h
  ./learn-opengl/camera.cpp
)

add_library(model
  STATIC
  ${HEADER_DIR}/learn-opengl/model.h
  ./learn-opengl/model.cpp
)
target_link_libraries(model PRIVATE assimp::assimp)