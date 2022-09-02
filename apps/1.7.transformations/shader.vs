#version 330 core                   // version and profile

layout(location = 0) in vec3 v_pos;  // layout of input data
layout(location = 1) in vec3 v_clr;
layout(location = 2) in vec2 v_tex_coord;

uniform mat4 trans;

out vec4 color;
out vec2 tex_coord;

void main() {
  gl_Position = trans * vec4(v_pos, 1.0);
  color = vec4(v_clr, 1.0);
  tex_coord = v_tex_coord;
}