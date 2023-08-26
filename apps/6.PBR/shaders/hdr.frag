#version 330
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;
uniform float exposure;

void main() {
  const float gamma = 2.2;
  vec3 result = texture(texture1, TexCoords).rgb;
  // Tone mapping.
  result = vec3(1.0) - exp(-result * exposure);
  // Gamma correct.
  result = pow(result, vec3(1.0 / gamma));
  FragColor = vec4(result, 1.0);
}