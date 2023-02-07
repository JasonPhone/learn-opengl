#version 330 core

out vec4 FragColor;

in vec3 fTexCoord;

uniform samplerCube skybox;

void main() {
  FragColor = texture(skybox, fTexCoord);
}