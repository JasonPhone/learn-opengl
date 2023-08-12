#version 330 core
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoords;

out VS_OUT {
  vec3 fragWorldPos;
  vec3 fragWorldNormal;
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
  vs_out.fragWorldNormal = normalMatrix * vNormal;
  if (reverseNormal)
    vs_out.fragWorldNormal *= -1;
  vs_out.texCoords = vTexCoords;
  vs_out.texWorldCoords = vec2(model * vec4(vTexCoords, 0, 1));
  gl_Position = proj * view * model * vec4(vPos, 1.0);
}