cmake_minimum_required(VERSION 3.5.0)

project(learn-opengl)

# set(3DPARTY_FOLDER ./extern)
# include(FetchContent)
# Set(FETCHCONTENT_QUIET FALSE)
# if(NOT TARGET assimp)
#   message(STATUS "============start config assimp============")
#   FetchContent_Declare(
#     assimp
#     GIT_REPOSITORY https://github.com/assimp/assimp.git
#     GIT_TAG v5.2.5

#     GIT_PROGRESS TRUE
#     SOURCE_DIR ${CMAKE_SOURCE_DIR}/extern/assimp
#   )

#   set(ASSIMP_FOLDER ${3DPARTY_FOLDER}/assimp)
#   set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE INTERNAL "")
#   set(ASSIMP_DOUBLE_PRECISION ON CACHE INTERNAL "")
#   set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE INTERNAL "")
#   set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE INTERNAL "")
#   set(ASSIMP_BUILD_TESTS OFF CACHE INTERNAL "")
#   set(ASSIMP_INSTALL OFF CACHE INTERNAL "")

#   FetchContent_MakeAvailable(assimp)
#   target_link_libraries(${TARGET_NAME} PUBLIC assimp)

#   set_target_properties(assimp PROPERTIES DEBUG_POSTFIX "_d")
#   set_target_properties(assimp PROPERTIES RELEASE_POSTFIX "_r")

#   set_ide_folder(assimp ${ASSIMP_FOLDER})
#   set_ide_folder(UpdateAssimpLibsDebugSymbolsAndDLLs ${ASSIMP_FOLDER})
#   set_ide_folder(zlibstatic ${ASSIMP_FOLDER})
# endif()
# find_package(OpenGL REQUIRED)
include_directories(
  ./extern
  ./include
  ./src
)

add_subdirectory(./apps)

add_subdirectory(./src)
