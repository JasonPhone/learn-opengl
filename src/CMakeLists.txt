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