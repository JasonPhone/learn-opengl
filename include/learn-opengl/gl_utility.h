/**
 * @file gl_utility.h
 * @author ja50n (zs_feng@qq.com)
 * @brief OpenGL utility functions
 * @version 0.1
 * @date 2022-08-17
 *
 */
#pragma once

#include <glad/glad.h>

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

/**
 * @brief Check if gl call causes error
 * @note NEVER call this before glfwInit()
 *
 * @param file filename where this check is called
 * @param line line number where this check is called
 * @return GLenum error code
 */
inline GLenum glCheckError_(const char* file, int line) {
  GLenum errorCode;
  while ((errorCode = glGetError()) != GL_NO_ERROR) {
    // clang-format off
    std::string error;
    switch (errorCode) {
      case GL_INVALID_ENUM:                   error = "INVALID_ENUM";
        break;
      case GL_INVALID_VALUE:                  error = "INVALID_VALUE";
        break;
      case GL_INVALID_OPERATION:              error = "INVALID_OPERATION";
        break;
      // not found
      // case GL_STACK_OVERFLOW:                 error = "STACK_OVERFLOW";
      //   break;
      // case GL_STACK_UNDERFLOW:                error = "STACK_UNDERFLOW";
      //   break;
      case GL_OUT_OF_MEMORY:                  error = "OUT_OF_MEMORY";
        break;
      case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";
        break;
    }
    // clang-format on
    std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    return errorCode;
  }
  // here will always be no error
  return GL_NO_ERROR;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

#define ASSERT(x) assert((x))
#define PEEK(x) std::cout << #x << ": " << x << "\n"
#define DEBUG std::cout
#define LOG std::cout
#define WARN std::cout << __FILE__ << ":" << __LINE__ << "\n  Warn: "
#define ERR std::cerr << __FILE__ << ":" << __LINE__ << "\n  Error: "

constexpr int FLOAT_SIZE = sizeof(float);
