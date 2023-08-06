#version 330 core
in vec4 fragPos;

uniform vec3 lightPos;
uniform float farPlane;

void main() {
  float lightDistance = length(fragPos.xyz - lightPos);
  // Map to [0, 1] range by dividing far_plane.
  lightDistance = 1.0 * lightDistance / farPlane;
  // Write to depth.
  gl_FragDepth = lightDistance;
}