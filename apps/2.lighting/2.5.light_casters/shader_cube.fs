#version 330 core

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  sampler2D emission;
  float shininess;
};
struct Light {
  vec3 position;

  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float constant;
  float linear;
  float quadratic;
  float cutoff_inner;
  float cutoff_outer;
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
  // vec3 light_dir = normalize(-lit.direction); // Parallel light
  float diff = max(dot(norm, light_dir), 0.0);
  diffuse = diff * lit.diffuse * texture(mat.diffuse, tex_coord).rgb;
  // Specular
  float spec_strength = 0.5;
  vec3 view_dir = normalize(view_pos - frag_pos);
  vec3 reflect_dir = reflect(-light_dir, norm);
  float spec = pow(max(dot(reflect_dir, view_dir), 0.0), mat.shininess);
  specular = spec * lit.specular * texture(mat.specular, tex_coord).rgb;

  vec3 emission = texture(mat.emission, tex_coord).rgb;

  // Attenuation
  float lit_dis = length(lit.position - frag_pos);
  float attenuation = 1.0 / (lit.constant + lit.linear * lit_dis + lit.quadratic * lit_dis * lit_dis);

  ambient *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;

  vec3 result;
  // Spotlight
  float theta = dot(light_dir, normalize(-lit.direction));
  float epsilon = lit.cutoff_inner - lit.cutoff_outer;
  float intensity = clamp((theta - lit.cutoff_outer) / epsilon, 0.0, 1.0);
  diffuse *= intensity;
  specular *= intensity;
  result = ambient + diffuse + specular + emission * spec;

  frag_color = vec4(result, 1.0);
}