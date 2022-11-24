#version 330 core

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  sampler2D emission;
  float shininess;
};
struct DirLight {
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};
struct PointLight {
  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float constant;
  float linear;
  float quadratic;
};
struct SpotLight {
  PointLight point_light;
  float cutoff_inner;
  float cutoff_outer;
  vec3 direction;
};
#define N_POINT_LIGHTS 4
uniform PointLight point_lights[N_POINT_LIGHTS];
uniform DirLight dir_light;
uniform SpotLight spot_light;
uniform vec3 view_pos;
uniform Material mat;

in vec3 frag_normal;
in vec3 frag_pos;
in vec2 tex_coord;

out vec4 frag_color;

vec3 get_DirLight(DirLight light, vec3 normal, vec3 view_dir);
vec3 get_PointLight(PointLight light, vec3 frag_pos, vec3 normal,
                    vec3 view_dir);
vec3 get_SpotLight(SpotLight light, vec3 frag_pos, vec3 normal, vec3 view_dir);

void main() {
  vec3 norm = normalize(frag_normal);
  vec3 view_dir = normalize(view_pos - frag_pos);

  vec3 result;
  // DirLight
  result += get_DirLight(dir_light, norm, view_dir);
  // PointLight
  for (int i = 0; i < N_POINT_LIGHTS; i++) {
      result += get_PointLight(point_lights[i], frag_pos, norm, view_dir);
  }
  // SpotLight
  result += get_SpotLight(spot_light, frag_pos, norm, view_dir);

  frag_color = vec4(result, 1.0);
}

vec3 get_DirLight(DirLight light, vec3 normal, vec3 view_dir) {
  vec3 light_dir = -normalize(light.direction);
  // Diffuse
  float diff = max(dot(normal, light_dir), 0.0);
  // Specular
  vec3 half_vec = normalize(view_dir + light_dir);
  float spec = pow(max(dot(normal, half_vec), 0.0), mat.shininess);

  vec3 ambient = light.ambient * texture(mat.diffuse, tex_coord).rgb;
  vec3 diffuse = light.diffuse * diff * texture(mat.diffuse, tex_coord).rgb;
  vec3 specular = light.specular * spec * texture(mat.specular, tex_coord).rgb;
  vec3 emission = spec * texture(mat.emission, tex_coord).rgb;

  return (ambient + diffuse + specular + emission);
}

vec3 get_PointLight(PointLight light, vec3 frag_pos, vec3 normal,
                    vec3 view_dir) {
  vec3 light_dir = normalize(light.position - frag_pos);
  // Diffuse
  float diff = max(dot(normal, light_dir), 0.0);
  // Specular
  vec3 half_vec = normalize(view_dir + light_dir);
  float spec = pow(max(dot(normal, half_vec), 0.0), mat.shininess);
  // Attenuation
  float light_dis = length(light.position - frag_pos);
  float attenuation = 1.0 / (light.constant + light.linear * light_dis +
                             light.quadratic * light_dis * light_dis);

  vec3 ambient = light.ambient * texture(mat.diffuse, tex_coord).rgb;
  vec3 diffuse = light.diffuse * diff * texture(mat.diffuse, tex_coord).rgb;
  vec3 specular = light.specular * spec * texture(mat.specular, tex_coord).rgb;
  vec3 emission = spec * texture(mat.emission, tex_coord).rgb;

  ambient *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;
  // Emission has no attenuation, to make a 'magic' feel.
  return (ambient + diffuse + specular + emission);
}

vec3 get_SpotLight(SpotLight light, vec3 frag_pos, vec3 normal, vec3 view_dir) {
  // Get the PointLight color
  vec3 result = get_PointLight(light.point_light, frag_pos, normal, view_dir);
  // Cut with spot test
  vec3 light_dir = normalize(light.point_light.position - frag_pos);
  float theta = dot(light_dir, normalize(-light.direction));
  float epsilon = light.cutoff_inner - light.cutoff_outer;
  float intensity = clamp((theta - light.cutoff_outer) / epsilon, 0.0, 1.0);
  result *= intensity;
  return result;
}