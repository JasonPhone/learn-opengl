#version 330 core
out vec4 fragColor;

in vec2 texCoords;

uniform sampler2D tex;
uniform float nearPlane;
uniform float farPlane;

// required when using a perspective projection matrix
float LinearizeDepth(float depth) {
  float z = depth * 2.0 - 1.0; // Back to NDC
  return (2.0 * nearPlane * farPlane) /
         (farPlane + nearPlane - z * (farPlane - nearPlane));
}

void main() {
  float depthValue = texture(tex, texCoords).r;
  // Projective.
  // fragColor = vec4(vec3(LinearizeDepth(depthValue) / far_plane), 1.0); //
  // Perspective.
  fragColor = vec4(vec3(depthValue), 1.0); // orthographic
}