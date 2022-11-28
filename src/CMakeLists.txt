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

add_library(texture
  STATIC
  ${HEADER_DIR}/learn-opengl/texture.h
  ./learn-opengl/texture.cpp
)
