#version 330 core

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  sampler2D emission;
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
in vec2 tex_coord;

out vec4 frag_color;

uniform vec3 view_pos;
uniform Material mat;
uniform Light lit;

void main() {
  // Phong shading
  vec3 ambient, diffuse, specular;
  // Ambient
  ambient = lit.ambient * texture(mat.diffuse, tex_coord).rgb;
  // Diffuse
  vec3 norm = normalize(normal);
  vec3 light_dir = normalize(lit.position - frag_pos);
  float diff = max(dot(norm, light_dir), 0.0);
  diffuse = diff * lit.diffuse * texture(mat.diffuse, tex_coord).rgb;
  // Specular
  float spec_strength = 0.5;
  vec3 view_dir = normalize(view_pos - frag_pos);
  vec3 reflect_dir = reflect(-light_dir, norm);
  float spec = pow(max(dot(reflect_dir, view_dir), 0.0), mat.shininess);
  specular = spec * lit.specular * texture(mat.specular, tex_coord).rgb;

  vec3 emission = texture(mat.emission, tex_coord).rgb;

  vec3 result = ambient + diffuse + specular + emission * spec;

  frag_color = vec4(result, 1.0);
}