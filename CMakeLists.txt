cmake_minimum_required(VERSION 3.5.0)

project(learn-opengl)

# find_package(OpenGL REQUIRED)
include_directories(
  ./extern
  ./include
  ./src
)

# add_executable(gl_learn ./src/glad.c ./src/main.cpp)

# target_link_libraries(gl_learn
# "${PROJECT_SOURCE_DIR}/lib/libglfw3.a"
# )
add_subdirectory(./apps/1.5.shader)
add_subdirectory(./apps/1.6.textures)
add_subdirectory(./apps/1.7.transformations)
add_subdirectory(./apps/1.8.coordinate.systems)
add_subdirectory(./apps/1.9.camera)
add_subdirectory(./apps/2.1.colors)
add_subdirectory(./apps/2.2.basic_lighting)
add_subdirectory(./apps/2.3.materials)
add_subdirectory(./apps/2.4.lighting_maps)
add_subdirectory(./apps/2.5.light_casters)
add_subdirectory(./apps/2.6.multiple_lights)

add_subdirectory(./apps/test)

add_subdirectory(./src)
