set(HEADER_DIR "${CMAKE_SOURCE_DIR}/include")

add_library(shader
  STATIC
  ${HEADER_DIR}/gl_learn/shader.h
  shader.cpp
)

add_library(stb_image
  STATIC
  ${HEADER_DIR}/stb_image.h
  stb_image.cpp
)