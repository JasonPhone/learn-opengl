#version 330 core

uniform vec3 obj_color;
uniform vec3 light_color;

out vec4 FragColor;

void main() {
  FragColor = vec4(obj_color * light_color, 1.0);
}