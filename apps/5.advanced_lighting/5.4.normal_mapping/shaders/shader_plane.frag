#version 330 core
out vec4 fragColor;

in GS_OUT {
  vec3 fragWorldPos;
  vec3 faceWorldNormal;
  vec2 texCoords;
}
fs_in;
in mat3 TBN;

uniform sampler2D texDiffuse;
uniform sampler2D normalMap;
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
  return inLight;
}

void main() {
  vec3 color = texture(texDiffuse, fs_in.texCoords).rgb;
  // Ambient
  vec3 colorAmbient = color * 0.05;
  // Diffuse
  vec3 wi = normalize(lightPos - fs_in.fragWorldPos);
  vec3 n = normalize(fs_in.faceWorldNormal);
  n = texture(normalMap, fs_in.texCoords).rgb;
  n = normalize(TBN * (n * 2.0 - 1.0));
  float diff = max(0, dot(n, wi));
  vec3 colorDiffuse = diff * color;
  // Specular
  vec3 wo = normalize(viewPos - fs_in.fragWorldPos);
  float spec = 0;
  // Blinn-Phong
  vec3 wHalf = normalize(wi + wo);
  spec = pow(max(0, dot(n, wHalf)), 64);
  vec3 colorSpecular = spec * vec3(0.3);

  float inLight = fragInLight(fs_in.fragWorldPos);
  fragColor = vec4(
      colorAmbient + colorDiffuse * inLight + colorSpecular * inLight, 1.0);
}