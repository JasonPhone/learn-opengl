#version 330 core
out vec4 fragColor;

in vec2 texCoords;

uniform sampler2D texture1;
uniform float nearPlane;
uniform float farPlane;

// required when using a perspective projection matrix
float linearizeDepth(float depth) {
  float z = depth * 2.0 - 1.0; // Back to NDC
  return (2.0 * nearPlane * farPlane) /
         (farPlane + nearPlane - z * (farPlane - nearPlane));
}

void main() {
  float depthValue = texture(texture1, texCoords).r;
  // Perspective.
  // fragColor = vec4(vec3(linearizeDepth(depthValue) / farPlane), 1.0);
  // Orthographic
  fragColor = vec4(vec3(depthValue), 1.0);
}