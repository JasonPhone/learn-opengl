#version 330 core
layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gDiffDpec;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main() {
  // All in world space.
  gPosition = FragPos;
  gNormal = normalize(Normal);
  gDiffDpec.rgb = texture(texture_diffuse1, TexCoords).rgb;
  gDiffDpec.a = texture(texture_specular1, TexCoords).r;
}