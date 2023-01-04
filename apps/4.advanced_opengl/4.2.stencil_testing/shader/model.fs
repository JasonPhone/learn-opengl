#version 330 core

uniform sampler2D texture1;

in vec2 fTexCoord;

out vec4 FragColor;

float near = 0.1, far = 100;

void main() {
  // Regular output
  FragColor = texture(texture1, fTexCoord);
}