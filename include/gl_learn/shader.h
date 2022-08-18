/**
 * @file shader.h
 * @author ja50n (zs_feng@qq.com)
 * @brief custom shader class
 * @version 0.1
 * @date 2022-08-18
 * 
 */
#pragma once

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

/**
 * @brief organize the shader program with default geometry shader
 *
 */
class Shader {
 public:
  /**
   * @brief Construct a new Shader object
   *
   * @param vertex_shader_path path to vertex shader
   * @param fragment_shader_path path to fragment shader
   */
  Shader(const char* vertex_shader_path, const char* fragment_shader_path);
  /**
   * @brief use this program in context
   */
  void use();
  /**
   * @brief Get the program id
   * 
   * @return GLUint shader program handle
   */
  GLuint get_program();

 private:
  /**
   * @brief id of shader program
   *
   */
  GLuint ID;
};