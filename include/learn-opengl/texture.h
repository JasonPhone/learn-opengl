/**
 * @file texture.h
 * @author ja50n (zs_feng@qq.com)
 * @brief Manage the image texture.
 * @version 0.1
 * @date 2022-10-25
 *
 */
#pragma once

#include <glad/glad.h>
#include <string>
#include <iostream>
#include "stb_image.h"

/**
 * @brief Texture class, currently only for 2D images.
 *
 */
class Texture2D {
 public:
  Texture2D() = delete;
  Texture2D(Texture2D &) = delete;
  /**
   * @brief Construct a new Texture 2D object
   *        and load the image file.
   *
   * @param path Path to image.
   */
  Texture2D(char const *path, GLint _color_mode = -1);
  /**
   * @brief Set the integer texture parameter
   *
   * @param target Texture type, 1D or 2D
   * @param pname Parameter name
   * @param param Value which parameter should take
   */
  void setParameteri(GLenum pname, GLint param) const;
  GLuint ID;

 protected:
 private:
  unsigned char *img_ptr;
  int img_w, img_h;
  GLint color_mode;
  /**
   * @brief Warpper for image loading library.
   *        Currently stb_image.
   * @note  The color mode should be specified
   *        or guessed through file loading.
   * @param path Path to image file.
   * @return true Image loading succeed.
   * @return false Image loading failed.
   */
  bool loadImage(char const *path, GLint _color_mode = -1);
  void freeImage();
};