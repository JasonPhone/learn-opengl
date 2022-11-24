#version 330 core

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};
struct Light {
  vec3 position;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

in vec3 normal;
in vec3 frag_pos;

out vec4 frag_color;

uniform vec3 view_pos;
uniform Material mat;
uniform Light lit;

void main() {
  // Phong shading
  vec3 ambient, diffuse, specular;
  // Ambient
  ambient = lit.ambient * mat.ambient;
  // Diffuse
  vec3 norm = normalize(normal);
  vec3 light_dir = normalize(lit.position - frag_pos);
  float diff = max(dot(norm, light_dir), 0.0);
  diffuse = diff * lit.diffuse * mat.diffuse;
  // Specular
  float spec_strength = 0.5;
  vec3 view_dir = normalize(view_pos - frag_pos);
  vec3 reflect_dir = reflect(-light_dir, norm);
  float spec = pow(max(dot(reflect_dir, view_dir), 0.0), mat.shininess);
  specular = spec * lit.specular * mat.specular;

  vec3 result = (ambient + diffuse + specular);

  frag_color = vec4(result, 1.0);
}