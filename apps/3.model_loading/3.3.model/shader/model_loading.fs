#version 330 core
out vec4 FragColor;

in vec2 fTexCoords;
in vec3 fPosition;
in vec3 fNormal;

uniform vec3 view_pos;

struct Material {
  sampler2D texture_diffuse1;
  sampler2D texture_diffuse2;

  sampler2D texture_specular1;
  sampler2D texture_specular2;

  sampler2D texture_normal1;
  sampler2D texture_normal2;

  sampler2D texture_height1;
  sampler2D texture_height2;
};
struct PointLight {
  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float att_const;
  float att_linear;
  float att_quadratic;

};
uniform Material material;
// uniform sampler2D texture_diffuse1;
uniform PointLight point_light;

vec3 get_PointLight(PointLight light, vec3 pos, vec3 normal, vec3 view_dir);

void main() {
  vec3 frag_normal = normalize(fNormal);
  vec3 view_dir = normalize(view_pos - fPosition);

  vec3 shading_result = get_PointLight(point_light, fPosition, frag_normal, view_dir);
  FragColor = vec4(shading_result, 1.0);
}

vec3 get_PointLight(PointLight light, vec3 pos, vec3 normal, vec3 view_dir) {
  vec3 light_dir = normalize(light.position - pos);
  // Diffuse
  float diff = max(dot(normal, light_dir), 0.0);
  // Specular
  vec3 half_vec = normalize(view_dir + light_dir);
  //  64 is random
  float spec = pow(max(dot(normal, half_vec), 0.0), 64);
  // Attenuation
  float light_dis = length(light.position - pos);
  float attenuation = 1.0 / (light.att_const + light.att_linear * light_dis + light.att_quadratic * light_dis * light_dis);
  // Result
  vec3 ambient = light.ambient * texture(material.texture_diffuse1, fTexCoords).rgb * attenuation;
  vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, fTexCoords).rgb * attenuation;
  vec3 specular = light.specular * spec * texture(material.texture_specular1, fTexCoords).rgb * attenuation;

  return (ambient + diffuse + specular);
}