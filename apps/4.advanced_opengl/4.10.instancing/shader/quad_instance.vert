#version 330 core
layout(location = 0) in vec3 vPos;
layout(location = 2) in vec2 vTexCoord;
layout(location = 3) in mat4 instanceMatrix;

out vec2 fTexCoord;

uniform mat4 projection;
uniform mat4 view;

void main() {
  gl_Position = projection * view * instanceMatrix * vec4(vPos, 1.0);
  fTexCoord = vTexCoord;
}