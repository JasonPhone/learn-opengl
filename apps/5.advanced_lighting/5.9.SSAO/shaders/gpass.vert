#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform bool inverse_normal;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

void main() {
  TexCoords = aTexCoords;

  mat3 normalMatrix = transpose(inverse(mat3(model)));
  Normal = normalize(normalMatrix * aNormal);
  if (inverse_normal)
    Normal = -Normal;

  FragPos = vec3(model * vec4(aPos, 1));

  gl_Position = projection * view * model * vec4(aPos, 1.0);
}