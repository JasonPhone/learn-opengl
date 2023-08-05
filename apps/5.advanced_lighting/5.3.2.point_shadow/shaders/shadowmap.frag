#version 330 core
in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

void main() {
  float lightDistance = length(FragPos.xyz - lightPos);
  // Map to [0, 1] range by dividing far_plane.
  lightDistance = 1.0 * lightDistance / far_plane;
  // Write to depth.
  gl_FragDepth = lightDistance;
}