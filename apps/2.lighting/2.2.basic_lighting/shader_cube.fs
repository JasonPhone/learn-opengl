#version 330 core

in vec3 normal;
in vec3 frag_pos;

out vec4 frag_color;

uniform vec3 obj_color;
uniform vec3 light_color;
uniform vec3 light_pos;
uniform vec3 view_pos;


void main() {
  // Phong shading
  vec3 ambient, diffuse, specular;
  // Ambient
  float ambient_strength = 0.1;
  ambient = ambient_strength * light_color;
  // Diffuse
  vec3 norm = normalize(normal);
  vec3 light_dir = normalize(light_pos - frag_pos);
  float diff = max(dot(norm, light_dir), 0.0);
  diffuse = diff * light_color;
  // Specular
  float spec_strength = 0.5;
  vec3 view_dir = normalize(view_pos - frag_pos);
  vec3 reflect_dir = reflect(-light_dir, norm);
  float spec = pow(max(dot(reflect_dir, view_dir), 0.0), 32);
  specular = spec_strength * spec * light_color;

  vec3 result = (ambient + diffuse + specular) * obj_color;

  frag_color = vec4(result , 1.0);
}