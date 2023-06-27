#version 330 core                   // version and profile

layout(location = 0) in vec3 v_position;  
layout(location = 1) in vec3 v_normal;

out vec3 normal; // Will be LERPed, but the 3 vertices have same normal
out vec3 frag_pos;

uniform mat4 normal_mat;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
  gl_Position = proj * view * model * vec4(v_position, 1.0);
  frag_pos = vec3(model * vec4(v_position, 1.0));
  normal = mat3(normal_mat) * v_normal;
}