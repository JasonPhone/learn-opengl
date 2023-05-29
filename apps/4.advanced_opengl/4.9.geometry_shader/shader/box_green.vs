#version 330 core
layout(location = 0) in vec3 vPosition;

layout(std140) uniform Matrices {
  mat4 projection;
  mat4 view;
};
uniform mat4 model;

out vec4 color;

void main() {
  gl_Position = projection * view * model * vec4(vPosition, 1.0);
  color = vec4(0, 1, 0, 1);
}