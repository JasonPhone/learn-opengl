/**
 * @file Shader.h
 * @author ja50n (zs_feng@qq.com)
 * @brief custom shader class
 * @version 0.1
 * @date 2022-08-18
 *
 */
#pragma once

#include <glad/glad.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

/// @brief organize the shader program with default geometry shader
class Shader {
 public:
  /// @brief Construct a new Shader object
  ///
  /// @param v_shader_path path to vertex shader
  /// @param f_shader_path path to fragment shader
  Shader(char const *v_shader_path, char const *f_shader_path);
  /// @brief Construct a new Shader object
  ///
  /// @param v_shader_path path to vertex shader
  /// @param g_shader_path path to geometry shader
  /// @param f_shader_path path to fragment shader
  Shader(char const *v_shader_path, char const *g_shader_path,
         char const *f_shader_path);
  /// @brief use this program in context
  void use() const;
  void setInt(char const *uniform_name, int value) const;
  void setBool(char const *uniform_name, bool value) const;
  void setFloat(char const *uniform_name, float value) const;
  void setVec2f(char const *uniform_name, float x, float y) const;
  void setVec2fv(char const *uniform_name, GLfloat const *ptr) const;
  void setVec3f(char const *uniform_name, float x, float y, float z) const;
  void setVec3fv(char const *uniform_name, GLfloat const *ptr) const;
  /// @brief Set the value of an existing matrix4 uniform
  ///
  /// @param uniform_name Name of this uniform
  /// @param ptr glm::value_ptr(.)
  void setMat4fv(char const *uniform_name, GLfloat const *ptr) const;
  /// @brief id of shader program
  GLuint ID;

 private:
  /// @brief read shader code from path and compile it
  ///
  /// @param path shader file
  /// @param shader_type type of shader, just pass the GL_VERTEX_SHADER or else
  /// @return GLuint shader handle
  GLuint loadAndCompile(const char *path, GLenum shader_type);
};