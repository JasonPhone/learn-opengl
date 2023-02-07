#version 330 core
out vec4 FragColor;

in vec2 fTexCoord;
in vec3 fNormal;
in vec3 fPosition;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform samplerCube skybox;
uniform vec3 camera_pos;

void main() {
  vec3 ray_in = normalize(fPosition - camera_pos);

  vec3 ray_reflect = reflect(ray_in, normalize(fNormal));
  vec4 reflect_clr = vec4(texture(skybox, ray_reflect).rgb, 1.0);

  float refract_ratio = 1.0 / 1.52;
  vec3 ray_refract = refract(ray_in, normalize(fNormal), refract_ratio);
  vec4 refract_clr = vec4(texture(skybox, ray_refract).rgb, 1.0);

  vec4 diffuse_clr = vec4(texture(texture_diffuse, fTexCoord).rgb, 1.0);

  float spec = texture(texture_specular, fTexCoord).r;

  FragColor = reflect_clr * spec * 0.7 + diffuse_clr * (0.7 - 0.7 * spec) + refract_clr * 0.3;
}