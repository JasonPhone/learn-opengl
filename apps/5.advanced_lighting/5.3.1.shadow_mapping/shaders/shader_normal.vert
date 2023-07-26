#version 330 core
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoords;

out VS_OUT { vec3 normal; }
vs_out;

uniform mat4 view;
uniform mat4 model;

void main() {
  gl_Position = view * model * vec4(vPos, 1.0);
  // Geom shader gets the data from viewing space.
  mat3 normalMatrix = mat3(transpose(inverse(view * model)));
  vs_out.normal = normalize(vec3(vec4(normalMatrix * vNormal, 0.0)));
}