#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

uniform mat4 shadowMats[6];

out vec4 fragPos;

void main() {
  for (int face = 0; face < 6; face++) {
    // Specify face of the cubemap to render.
    gl_Layer = face;
    for (int i = 0; i < 3; i++) {
      // Used to calculate the linear distance.
      fragPos = gl_in[i].gl_Position;
      // Map to the cubemap. One triangle goes to all 6 faces.
      gl_Position = shadowMats[face] * fragPos;
      EmitVertex();
    }
    EndPrimitive();
  }
}