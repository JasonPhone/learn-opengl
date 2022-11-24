#version 330 core                   // version and profile
layout(location = 0) in vec3 aPos;  // layout of input data
layout(location = 1) in vec3 aClr;
out vec4 color;
void main() {
  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
  color = vec4(aClr, 1.0);
}