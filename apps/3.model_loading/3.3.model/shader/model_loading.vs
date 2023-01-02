#version 330 core
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;

out vec2 fTexCoords;
out vec3 fPosition;
out vec3 fNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  fTexCoords = vTexCoord;
  fPosition = vPosition;
  fNormal = vNormal;

  gl_Position = projection * view * model * vec4(vPosition, 1.0);
}