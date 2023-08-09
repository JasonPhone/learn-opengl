#version 330 core
out vec4 fragColor;

in GS_OUT {
  vec3 fragWorldPos;
  vec3 faceWorldNormal;
  vec2 texCoords;
}
fs_in;
in vec3 fT, fB;

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
uniform Material material;

uniform samplerCube shadowMap;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float farPlane;

float fragInLight(vec3 fragPosition) {
  vec3 lightToFrag = fragPosition - lightPos;
  float firstHit = texture(shadowMap, lightToFrag).r;
  // [0, 1] to real distance.
  firstHit *= farPlane;
  float curHit = length(lightToFrag);
  // [0.20, 0.30] adaptive bias.
  float bias =
      max(0.030 * (1.0 - dot(fs_in.faceWorldNormal, -normalize(lightToFrag))),
          0.020);

  float inLight = curHit - bias > firstHit ? 0.0 : 1.0;
  return 1.0;
  return inLight;
}

void main() {
  vec3 color = texture(material.texture_diffuse1, fs_in.texCoords).rgb;
  // Use interpolated normal.
  vec3 N = fs_in.faceWorldNormal;
  vec3 T = normalize(fT - dot(fT, N) * N);
  vec3 B = cross(N, T);
  mat3 TBN = mat3(T, B, N);
  // Ambient
  vec3 colorAmbient = color * 0.05;

  // Diffuse
  vec3 wi = normalize(lightPos - fs_in.fragWorldPos);
  vec3 ddn = normalize(texture(material.texture_normal1, fs_in.texCoords).rgb);
  ddn = normalize(TBN * (ddn * 2.0 - 1.0));
  float diff = max(0, dot(ddn, wi));
  vec3 colorDiffuse = diff * color;

  // Specular
  vec3 wo = normalize(viewPos - fs_in.fragWorldPos);
  float spec = 0;
  //// Blinn-Phong
  vec3 wHalf = normalize(wi + wo);
  spec = pow(max(0, dot(ddn, wHalf)), 64);
  vec3 colorSpecular =
      spec * texture(material.texture_specular1, fs_in.texCoords).rgb;

  float inLight = fragInLight(fs_in.fragWorldPos);
  fragColor = vec4(
      colorAmbient + colorDiffuse * inLight + colorSpecular * inLight, 1.0);
}