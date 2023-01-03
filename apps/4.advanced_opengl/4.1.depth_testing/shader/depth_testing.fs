#version 330 core

uniform sampler2D texture1;

in vec2 fTexCoord;

out vec4 FragColor;

float near = 0.1, far = 100;

float get_linear_depth(float depth) {
  float z = depth * 2.0 - 1.0; // Back to NDC
  return (2.0 * near * far) / (far + near - z * (far - near));
}

void main() {
  // Turn perspective depth to linear depth
  float depth = (get_linear_depth(gl_FragCoord.z) - near) / (far - near);
  FragColor = vec4(vec3(depth), 1.0);

  // Regular output
  FragColor = texture(texture1, fTexCoord);
}