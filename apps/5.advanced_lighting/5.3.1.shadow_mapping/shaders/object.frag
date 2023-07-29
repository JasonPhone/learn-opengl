#version 330 core
out vec4 fragColor;

in VS_OUT {
  vec3 fragPos;
  vec3 normal;
  vec2 texCoords;
  vec4 fragPosInLightSpace;
}
fs_in;

uniform sampler2D texDiffuse;
uniform sampler2D texShadow;
uniform vec3 lightPos;
uniform vec3 viewPos;

float fragInLight(vec4 fragPosLightSpace) {
  // Perspective divide.
  vec3 projCoord = fragPosLightSpace.xyz / fragPosLightSpace.w;
  // [-1, 1] to [0, 1].
  projCoord = projCoord * 0.5 + 0.5;
  float firstHit = texture(texShadow, projCoord.xy).r;
  float currentHit = projCoord.z;
  vec3 normal = normalize(fs_in.normal);
  vec3 lightDir = normalize(lightPos - fs_in.fragPos);
  float bias = max(0.002 * (1.0 - dot(normal, lightDir)), 0.0002);
  return currentHit - bias > firstHit ? 0 : 1;
}

void main() {
  vec3 color = texture(texDiffuse, fs_in.texCoords).rgb;
  // Ambient
  vec3 colorAmbient = color * 0.05;
  // Diffuse
  vec3 wi = normalize(lightPos - fs_in.fragPos);
  vec3 n = normalize(fs_in.normal);
  float diff = max(0, dot(n, wi));
  vec3 colorDiffuse = diff * color;
  // Specular
  vec3 wo = normalize(viewPos - fs_in.fragPos);
  float spec = 0;
  // Blinn-Phong
  vec3 wHalf = normalize(wi + wo);
  spec = pow(max(0, dot(n, wHalf)), 64);
  vec3 colorSpecular = spec * vec3(0.3);

  float inLight = fragInLight(fs_in.fragPosInLightSpace);
  fragColor = vec4(
      colorAmbient + colorDiffuse * inLight + colorSpecular * inLight, 1.0);
}