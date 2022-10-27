#version 330 core                   // version and profile

layout(location = 0) in vec3 v_pos;  // layout of input data

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
  gl_Position = proj * view * model * vec4(v_pos, 1.0);
}