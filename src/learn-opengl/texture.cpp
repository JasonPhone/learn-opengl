/**
 * @file texture.cpp
 * @author ja50n (zs_feng@qq.com)
 * @brief Implemention of Texture class
 * @version 0.1
 * @date 2022-10-25
 *
 */
#include "learn-opengl/texture.h"

Texture2D::Texture2D(char const *path, GLint _color_mode) {
  bool true_if_loadImage_ok = loadImage(path, _color_mode);
  if (!true_if_loadImage_ok) {
    ID = 0;
    std::cerr << "Texture2D::Texture2D: Failed to load image file " << path << "\n";
    return;
  }
  glGenTextures(1, &ID);
  glBindTexture(GL_TEXTURE_2D, ID);
  glTexImage2D(GL_TEXTURE_2D, 0, color_mode, img_w, img_h, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, img_ptr);
  glGenerateMipmap(GL_TEXTURE_2D);
  freeImage();
}

void Texture2D::setParameteri(GLenum pname, GLint param) const {
  glBindTexture(GL_TEXTURE_2D, ID);
  glTexParameteri(GL_TEXTURE_2D, pname, param);
  glBindTexture(GL_TEXTURE_2D, 0);
}

bool Texture2D::loadImage(char const *path, GLint _color_mode) {
  int img_channel;
  bool true_if_load_ok = false;
  img_ptr = stbi_load(path, &img_w, &img_h, &img_channel, 0);
  if (!img_ptr) {
    std::cerr << "Texture2D::loadImage: Empty image pointer.\n";
    true_if_load_ok = false;
  } else {
    switch (img_channel) {
      case 3:
        color_mode = GL_RGB;
        true_if_load_ok = true;
        break;
      case 4:
        color_mode = GL_RGBA;
        true_if_load_ok = true;
        break;
      default:
        std::cerr << "Texture2D::loadImage: Unknown image channel: "
                  << img_channel << "\n";
        true_if_load_ok = false;
    }
  }
  return true_if_load_ok;
}

void Texture2D::freeImage() {
  if (img_ptr) stbi_image_free(img_ptr);
  img_ptr = nullptr;
}