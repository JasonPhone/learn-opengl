#version 330 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoords;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

void main() { gl_Position = proj * view * model * vec4(vPos, 1.0); }