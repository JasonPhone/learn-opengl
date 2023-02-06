#version 330 core

uniform sampler2D texture1;

in vec2 fTexCoord;

out vec4 FragColor;

void main() {
  // Regular output
  vec3 tex_color = texture(texture1, fTexCoord).rgb;
  // float avg = (0.2126 * tex_color.r + 0.7152 * tex_color.g + 0.0722 * tex_color.b);
  float avg = (tex_color.r + tex_color.g + tex_color.b) / 3.0;
  FragColor = vec4(avg, avg, avg, 1.0);
}