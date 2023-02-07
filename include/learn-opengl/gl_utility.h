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
#include <vector>
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

/**
 * @brief Simple logger.
 *
 */
#define LOG std::cerr << __FILE__ << "(" << __LINE__ << ")"

constexpr int FLOAT_SIZE = sizeof(float);

/**
 * @brief Load a texture from image using stb_image.
 *
 * @param path Path to image.
 * @param warp_type S and T warp type. `GL_REPEAT`, `GL_MIRRORED_REPEAT`,
 * `CLAMP_TO_EDGE` and `GL_CLAMP_TO_BORDER`. `GL_REPEAT` by default.
 * @return unsigned int ID of texture.
 */
GLuint load_texture(char const* path, GLenum warp_type = GL_REPEAT) {
  GLuint texture_id;
  glGenTextures(1, &texture_id);

  int img_w, img_h, img_channels;
  unsigned char* data = stbi_load(path, &img_w, &img_h, &img_channels, 0);
  if (data) {
    GLenum format = GL_RED;
    switch (img_channels) {
      case 1: {
        format = GL_RED;
        break;
      }
      case 3: {
        format = GL_RGB;
        break;
      }
      case 4: {
        format = GL_RGBA;
        break;
      }
    }
    // For sanity check.
    GLenum warp = GL_REPEAT;
    switch (warp_type) {
      case GL_REPEAT: {
        warp = GL_REPEAT;
        break;
      }
      case GL_MIRRORED_REPEAT: {
        warp = GL_MIRRORED_REPEAT;
        break;
      }
      case GL_CLAMP_TO_EDGE: {
        warp = GL_CLAMP_TO_EDGE;
        break;
      }
      case GL_CLAMP_TO_BORDER: {
        warp = GL_CLAMP_TO_BORDER;
        break;
      }
    }

    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, format, img_w, img_h, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, warp);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, warp);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  } else {
    LOG << "load_texture: Failed to load texture at path: " << path;
    stbi_image_free(data);
  }
  return texture_id;
}

/**
 * @brief Load cubemap from image files.
 *
 * @param paths Vector storing paths to images.
 * @return GLuint Cubemap ID.
 */
GLuint load_cubemap(std::vector<std::string> const& paths) {
  GLuint texture_id = 0;
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

  int img_w, img_h, img_channels;
  for (unsigned int i = 0; i < paths.size(); i++) {
    unsigned char* data =
        stbi_load(paths[i].c_str(), &img_w, &img_h, &img_channels, 0);
    if (data) {
      GLenum format = GL_RED;
      switch (img_channels) {
        case 1: {
          format = GL_RED;
          break;
        }
        case 3: {
          format = GL_RGB;
          break;
        }
        case 4: {
          format = GL_RGBA;
          break;
        }
      }
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, img_w, img_h,
                   0, format, GL_UNSIGNED_BYTE, data);
    } else {
      LOG << "load_cubemap: Fail to load cubemap at path " << paths[i]
          << std::endl;
    }
    stbi_image_free(data);
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  return texture_id;
}