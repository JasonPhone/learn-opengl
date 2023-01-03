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
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include "stb_image.h"
/**
 * @brief Check if gl call causes error
 * @note NEVER call this before glfwInit()
 *
 * @param file filename where this check is called
 * @param line line number where this check is called
 * @return GLenum error code
 */
GLenum glCheckError_(const char* file, int line) {
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

void LOG_(const char* file, int line, const char* prompt, const char* content) {
  std::cerr << file << "(" << line << "): " << prompt  << " " << content << std::endl;
}
#define LOG(prompt, content) LOG_(__FILE__, __LINE__, prompt, content)

unsigned int load_texture(char const *path) {
  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrComponents;
  unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
  if (data) {
    GLenum format;
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  } else {
    LOG("loadTexture: Texture failed to load at path:", path);
    stbi_image_free(data);
  }
  return textureID;
}