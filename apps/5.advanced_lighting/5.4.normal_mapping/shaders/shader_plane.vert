#version 330 core
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoords;
/*
vert
  gl_Position = M * vPos


geom
  gl_Position = P * V * gl_Position and emit.
  calculate TBN, use three texCoords

frag
  use TBN.

*/

out VS_OUT {
  vec3 fragWorldPos;
  vec3 faceWorldNormal;
  vec2 texCoords;
  vec2 texWorldCoords;
}
vs_out;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
uniform bool reverseNormal;

void main() {
  // World space.
  vs_out.fragWorldPos = vec3(model * vec4(vPos, 1));
  mat3 normalMatrix = mat3(transpose(inverse(model)));
  vs_out.faceWorldNormal = normalMatrix * vNormal;
  if (reverseNormal)
    vs_out.faceWorldNormal *= -1;
  vs_out.texCoords = vTexCoords;
  vs_out.texWorldCoords = vec2(model * vec4(vTexCoords, 0, 1));
  gl_Position = proj * view * model * vec4(vPos, 1.0);
}