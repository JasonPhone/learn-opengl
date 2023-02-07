#version 330 core

layout (location = 0) in vec3 vPosition;

out vec3 fTexCoord;

uniform mat4 projection;
uniform mat4 view;

void main() {
  fTexCoord = vPosition; // Used to sample cubemap directly.
  gl_Position = projection * view * vec4(vPosition, 1.0);
  gl_Position = gl_Position.xyww;
}