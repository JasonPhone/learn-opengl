#version 330 core
in vec4 color;
in vec2 tex_coord;

uniform sampler2D texture0;
uniform sampler2D texture1;

out vec4 FragColor;

void main() {
  FragColor = mix(texture(texture0, tex_coord), texture(texture1, tex_coord), 0.2);
}