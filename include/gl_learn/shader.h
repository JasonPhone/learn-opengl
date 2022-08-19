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
   * @param v_shader_path path to vertex shader
   * @param f_shader_path path to fragment shader
   */
  Shader(const char* v_shader_path, const char* f_shader_path);
  /**
   * @brief use this program in context
   */
  void use() const;
  /**
   * @brief Set the value of a existing int uniform
   *
   * @param uniform_name name of this uniform
   * @param value value to set
   */
  void set_int(const std::string &uniform_name, int value) const;
  /**
   * @brief Set the value of a existing bool uniform
   *
   * @param uniform_name name of this uniform
   * @param value value to set
   */
  void set_bool(const std::string &uniform_name, bool value) const;
  /**
   * @brief Set the value of a existing float uniform
   *
   * @param uniform_name name of this uniform
   * @param value value to set
   */
  void set_float(const std::string &uniform_name, float value) const;
  /**
   * @brief id of shader program
   *
   */
  GLuint ID;

 private:
  /**
   * @brief read shader code from path and compile it
   *
   * @param path shader file
   * @param shader_type type of shader, just pass the GL_VERTEX_SHADER or else
   * @return GLuint shader handle
   */
  GLuint read_and_compile(const char* path, GLenum shader_type);
};