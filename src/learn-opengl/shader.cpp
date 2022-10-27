/**
 * @file shader.cpp
 * @author ja50n (zs_feng@qq.com)
 * @brief implemention of custom shader class
 * @version 0.1
 * @date 2022-08-18
 *
 */

#include "learn-opengl/shader.h"
Shader::Shader(const char* v_shader_path, const char* f_shader_path) {
  // read and compile two shaders
  GLuint v_shader = read_and_compile(v_shader_path, GL_VERTEX_SHADER);
  GLuint f_shader = read_and_compile(f_shader_path, GL_FRAGMENT_SHADER);

  // link as shader program
  ID = glCreateProgram();
  glAttachShader(ID, v_shader);
  glAttachShader(ID, f_shader);
  glLinkProgram(ID);

  int success;
  char info_log[512];
  glGetProgramiv(ID, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(ID, sizeof(info_log), NULL, info_log);
    std::cerr << "Shader::Shader():"
              << "\n\tShader linking error\n\t" << info_log << std::endl;
  }
  glDeleteShader(v_shader);
  glDeleteShader(f_shader);
}

GLuint Shader::read_and_compile(const char* path, GLenum shader_type) {
  // reading
  std::string shader_str;
  std::ifstream shader_file;
  // enable error states who are going to trigger the exceptions
  shader_file.exceptions(std::fstream::badbit | std::fstream::failbit);
  try {
    // open file
    shader_file.open(path);
    // use a string stream to receive the shader code
    std::stringstream shader_stream;
    shader_stream << shader_file.rdbuf();
    // close file
    shader_file.close();
    // code to string
    shader_str = shader_stream.str();
  } catch (std::ifstream::failure e) {
    std::cerr << "Shader::read_and_compile():"
              << "\n\tCannot read shader file: " << path << "\n\t" << e.what()
              << std::endl;
  }
  const char* shader_code = shader_str.c_str();

  // compiling
  GLuint shader;
  int success;
  char info_log[512];
  shader = glCreateShader(shader_type);
  glShaderSource(shader, 1, &shader_code, NULL);
  glCompileShader(shader);

  // if any compiling error
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, sizeof(info_log), NULL, info_log);
    std::cerr << "Shader::read_and_compile():"
              << "\n\tShader compiling error\n\t" << info_log << std::endl;
  }
  return shader;
}

void Shader::use() const { glUseProgram(ID); }

void Shader::set_int(char const* uniform_name, int value) const {
  glUniform1i(glGetUniformLocation(ID, uniform_name), value);
}
void Shader::set_bool(char const* uniform_name, bool value) const {
  glUniform1i(glGetUniformLocation(ID, uniform_name),
              static_cast<int>(value));
}
void Shader::set_float(char const* uniform_name, float value) const {
  glUniform1f(glGetUniformLocation(ID, uniform_name), value);
}
void Shader::set_vec3(char const* uniform_name, float x, float y, float z) const {
  glUniform3f(glGetUniformLocation(ID, uniform_name), x, y, z);
}
void Shader::set_mat4fv(char const* uniform_name, GLfloat const* ptr) const {
  glUniformMatrix4fv(glGetUniformLocation(ID, uniform_name), 1, GL_FALSE, ptr);
}