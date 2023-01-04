#version 330 core

uniform sampler2D texture1;

in vec2 fTexCoord;

out vec4 FragColor;

void main() {
  FragColor = vec4(0.7, 0.4, 0.4, 1.0);
}