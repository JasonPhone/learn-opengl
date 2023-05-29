cmake_minimum_required(VERSION 3.5)

project(imgui
  VERSION 1.89.4
  LANGUAGES CXX
  DESCRIPTION "ImGui as library."
  HOMEPAGE_URL "https://github.com/ocornut/imgui"
)

set(IMGUI_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(IMGUI_BACKEND_DIR ${CMAKE_CURRENT_SOURCE_DIR}/backend)

# Headers
set(IMGUI_PUBLIC_HEADERS
  ${IMGUI_SRC_DIR}/imgui.h
  ${IMGUI_SRC_DIR}/imconfig.h
  ${IMGUI_SRC_DIR}/imgui_internal.h
  ${IMGUI_SRC_DIR}/imstb_rectpack.h
  ${IMGUI_SRC_DIR}/imstb_textedit.h
  ${IMGUI_SRC_DIR}/imstb_truetype.h
)
# Backend headers
list(APPEND IMGUI_PUBLIC_HEADERS
  ${IMGUI_BACKEND_DIR}/imgui_impl_glfw.h
  ${IMGUI_BACKEND_DIR}/imgui_impl_opengl3.h
  ${IMGUI_BACKEND_DIR}/imgui_impl_opengl3_loader.h
)

# Source files
set(IMGUI_PUBLIC_SOURCES
  ${IMGUI_SRC_DIR}/imgui.cpp
  ${IMGUI_SRC_DIR}/imgui_draw.cpp
  ${IMGUI_SRC_DIR}/imgui_widgets.cpp
  ${IMGUI_SRC_DIR}/imgui_demo.cpp
  ${IMGUI_SRC_DIR}/imgui_tables.cpp
)
# Backend source files
list(APPEND IMGUI_PUBLIC_SOURCES
  ${IMGUI_BACKEND_DIR}/imgui_impl_glfw.cpp
  ${IMGUI_BACKEND_DIR}/imgui_impl_opengl3.cpp
)

# Fonts
# file(GLOB FONTS ${IMGUI_SRC_DIR}/fonts/*.ttf)

add_library(${PROJECT_NAME}
  STATIC
  ${IMGUI_PUBLIC_HEADERS}
  ${IMGUI_PUBLIC_SOURCES}
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
  ${IMGUI_SRC_DIR}
  ${IMGUI_BACKEND_DIR}
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
  "${CMAKE_SOURCE_DIR}/lib/libglfw3.a"
)
