set(HEADER_DIR "${CMAKE_SOURCE_DIR}/include/gl_learn")

add_library(shader shader.cpp ${HEADER_DIR}/shader.h)

add_library(gl_error_checker gl_error_checker.cpp ${HEADER_DIR}/gl_error_checker.h)
