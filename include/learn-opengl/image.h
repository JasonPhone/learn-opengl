/**
 * @file image.h
 * @author ja50n (zs_feng@qq.com)
 * @brief Image utilities. For now to isolate stb_image.h
 *
 * @version 0.1
 * @date 2023-07-19
 *
 */
#pragma once
#include <glad/glad.h>

#include <vector>

void flipVerticalOnLoad(int flag_true_if_flip);
unsigned char* loadImage(const char* file_name, int* x, int* y, int* channel,
                         int desire_channel);
void freeImage(void* image_ptr);

#define load_texture loadTexture
/**
 * @brief Load a texture from image using stb_image.
 *
 * @param path Path to image.
 * @param warp_type S and T warp type. `GL_REPEAT`, `GL_MIRRORED_REPEAT`,
 * `CLAMP_TO_EDGE` and `GL_CLAMP_TO_BORDER`. `GL_REPEAT` by default.
 * @return unsigned int ID of texture.
 */
GLuint loadTexture(char const* path, GLenum warp_type = GL_REPEAT);
/**
 * @brief Load cubemap from image files.
 *
 * @param paths Vector storing paths to images.
 * @return GLuint Cubemap ID.
 */
GLuint loadCubemap(std::vector<std::string> const& paths);
