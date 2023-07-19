set(HEADER_DIR "${CMAKE_SOURCE_DIR}/include")
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")

add_library(Shader
  STATIC

  # ${HEADER_DIR}/learn-opengl/Shader.h
  ${SOURCE_DIR}/learn-opengl/Shader.cpp
)

add_library(Camera
  STATIC

  # ${HEADER_DIR}/learn-opengl/Camera.h
  ${SOURCE_DIR}/learn-opengl/Camera.cpp
)

add_library(Model
  STATIC

  # ${HEADER_DIR}/learn-opengl/Model.h
  ${SOURCE_DIR}/learn-opengl/Model.cpp
)
target_link_libraries(Model PRIVATE image)

add_library(image
  STATIC

  # ${HEADER_DIR}/learn-opengl/image.h
  ${SOURCE_DIR}/learn-opengl/image.cpp
)