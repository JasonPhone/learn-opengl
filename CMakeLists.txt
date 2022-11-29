cmake_minimum_required(VERSION 3.5.0)

project(learn-opengl)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/)
# set(CMAKE_BUILD_RPATH ${CMAKE_SOURCE_DIR}/bin/)
# set(CMAKE_INSTALL_RPATH ${CMAKE_SOURCE_DIR}/bin/)

set(3DPARTY_FOLDER ./extern)
include(FetchContent)
Set(FETCHCONTENT_QUIET FALSE)
if(NOT TARGET assimp)
  message(STATUS "============start config assimp============")
  FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://gitclone.com/github.com/assimp/assimp.git
    GIT_TAG v5.2.5

    GIT_PROGRESS TRUE
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/extern/assimp
    # BINARY_DIR ${CMAKE_SOURCE_DIR}/bin
  )

  set(ASSIMP_FOLDER ${3DPARTY_FOLDER}/assimp)
  set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE INTERNAL "")
  set(ASSIMP_DOUBLE_PRECISION ON CACHE INTERNAL "")
  set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE INTERNAL "")
  set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE INTERNAL "")
  set(ASSIMP_BUILD_TESTS OFF CACHE INTERNAL "")
  set(ASSIMP_INSTALL OFF CACHE INTERNAL "")

  FetchContent_MakeAvailable(assimp)

  set_target_properties(assimp PROPERTIES DEBUG_POSTFIX "_debug")
  set_target_properties(assimp PROPERTIES RELEASE_POSTFIX "_release")

endif()
find_package(OpenGL REQUIRED)
include_directories(
  ./extern
  ./include
  ./src
)

add_subdirectory(./apps)

add_subdirectory(./src)
