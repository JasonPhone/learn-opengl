set(HEADER_DIR "${CMAKE_SOURCE_DIR}/include")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/)

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
