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

unsigned int load_texture(char const *path) {
  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrComponents;
  stbi_set_flip_vertically_on_load(true);
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
  } else {
    std::cout << "Texture failed to load at path: " << path << std::endl;
  }
  stbi_image_free(data);

  return textureID;
}

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
  Texture2D(char const *path);
  /**
   * @brief Set the integer texture parameter
   *
   * @param target Texture type, 1D or 2D
   * @param pname Parameter name
   * @param param Value which parameter should take
   */
  // void setParameteri(GLenum pname, GLint param) const;
  GLuint ID;

 protected:
 private:
  unsigned char *img_ptr;
  int img_w, img_h;
  GLenum color_mode;
  /**
   * @brief Warpper for image loading library.
   *        Currently stb_image.
   * @note  The color mode should be specified
   *        or guessed through file loading.
   * @param path Path to image file.
   * @return true Image loading succeed.
   * @return false Image loading failed.
   */
  bool loadImage(char const *path);
  void freeImage();
};