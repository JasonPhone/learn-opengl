#version 330 core

uniform sampler2D texture1;

in vec2 fTexCoord;

out vec4 FragColor;

float near = 0.1, far = 100;

void main() {
  // Regular output
  vec4 tex_color = texture(texture1, fTexCoord);
  // if (tex_color.a < 0.1) discard;
  FragColor = tex_color;
}