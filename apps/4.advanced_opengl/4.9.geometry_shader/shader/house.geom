#version 330 core
layout(points) in;
layout(triangle_strip, max_vertices = 5) out;
in VS_OUT {
    vec3 color;
} geom_in[];
out vec3 fColor;

void build_house(vec4 position) {
  fColor = geom_in[0].color; // All vertices will use this.
  gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0); // bottom-left
  EmitVertex();
  gl_Position = position + vec4(0.2, -0.2, 0.0, 0.0); // bottom-right
  EmitVertex();
  gl_Position = position + vec4(-0.2, 0.2, 0.0, 0.0); // top-left
  EmitVertex();
  gl_Position = position + vec4(0.2, 0.2, 0.0, 0.0); // top-right
  EmitVertex();
  gl_Position = position + vec4(0.0, 0.4, 0.0, 0.0); // top-mid
  fColor = vec3(1, 1, 1);
  EmitVertex();
  EndPrimitive();
}
void main() { build_house(gl_in[0].gl_Position); }