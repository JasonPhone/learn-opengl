#version 330 core
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoords;

// declare an interface block; see 'Advanced GLSL' for what these are.
out VS_OUT {
  vec3 fragPos;
  vec3 normal;
  vec2 texCoords;
  vec4 fragPosInLightSpace;
}
vs_out;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMat;

void main() {
  // World space.
  vs_out.fragPos = vec3(model * vec4(vPos, 1));
  vs_out.fragPosInLightSpace = lightSpaceMat * vec4(vPos, 1);
  mat3 normalMatrix = mat3(transpose(inverse(model)));
  vs_out.normal = normalMatrix * vNormal;
  vs_out.texCoords = vTexCoords;

  gl_Position = proj * view * model * vec4(vPos, 1.0);
}