/**
 * @file image.cpp
 * @author ja50n (zs_feng@qq.com)
 * @brief Impl of image.h
 *
 * @version 0.1
 * @date 2023-07-19
 *
 */
#include "learn-opengl/image.h"

#include "learn-opengl/gl_utility.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void flipVerticalOnLoad(int flag_true_if_flip) {
  stbi_set_flip_vertically_on_load(flag_true_if_flip);
}
unsigned char* loadImage(const char* file_name, int* x, int* y, int* channel,
                         int desire_channel) {
  return stbi_load(file_name, x, y, channel, desire_channel);
}
void freeImage(void* image_ptr) { stbi_image_free(image_ptr); }
/**
 * @brief Load a texture from image using stb_image.
 *
 * @param path Path to image.
 * @param warp_type S and T warp type. `GL_REPEAT`, `GL_MIRRORED_REPEAT`,
 * `CLAMP_TO_EDGE` and `GL_CLAMP_TO_BORDER`. `GL_REPEAT` by default.
 * @return unsigned int ID of texture.
 */
GLuint loadTexture(char const* path, GLenum warp_type) {
  GLuint texture_id;
  glGenTextures(1, &texture_id);

  int img_w, img_h, img_channels;
  unsigned char* data = loadImage(path, &img_w, &img_h, &img_channels, 0);
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

    freeImage(data);
  } else {
    LOG << "loadTexture: Failed to load texture at path: " << path;
    freeImage(data);
  }
  return texture_id;
}

/**
 * @brief Load cubemap from image files.
 *
 * @param paths Vector storing paths to images.
 * @return GLuint Cubemap ID.
 */
GLuint loadCubemap(std::vector<std::string> const& paths) {
  GLuint texture_id = 0;
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

  int img_w, img_h, img_channels;
  for (unsigned int i = 0; i < paths.size(); i++) {
    unsigned char* data =
        loadImage(paths[i].c_str(), &img_w, &img_h, &img_channels, 0);
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
      LOG << "loadCubemap: Fail to load cubemap at path " << paths[i]
          << std::endl;
    }
    freeImage(data);
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  return texture_id;
}