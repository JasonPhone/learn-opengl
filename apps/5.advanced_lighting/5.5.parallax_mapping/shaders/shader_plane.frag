#version 330 core
out vec4 fragColor;

in GS_OUT {
  vec3 fragWorldPos;
  vec3 fragWorldNormal;
  vec2 texCoords;
}
fs_in;
in vec3 fT, fB;

uniform sampler2D texDiffuse;
uniform sampler2D mapNormal;
uniform sampler2D mapDisplace;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float farPlane;
uniform float heightScale;
uniform int parallaxMethod;

// Original.
vec2 parallaxMapping1(vec2 texCoord, vec3 viewDir);
// Step marching.
vec2 parallaxMapping2(vec2 texCoord, vec3 viewDir);
//
vec2 parallaxMapping3(vec2 texCoord, vec3 viewDir);

void main() {
  // Use interpolated normal.
  vec3 N = fs_in.fragWorldNormal;
  vec3 T = normalize(fT - dot(fT, N) * N);
  vec3 B = cross(N, T);
  mat3 TBN = mat3(T, B, N);
  // Vars, all tangent space.
  vec3 wi = normalize(TBN * (lightPos - fs_in.fragWorldPos));
  vec3 wo = normalize(TBN * (viewPos - fs_in.fragWorldPos));
  vec2 texCoordsDisplaced;
  if (parallaxMethod == 1)
    texCoordsDisplaced = parallaxMapping1(fs_in.texCoords, wo);
  else if (parallaxMethod == 2)
    texCoordsDisplaced = parallaxMapping2(fs_in.texCoords, wo);
  else if (parallaxMethod == 3)
    texCoordsDisplaced = parallaxMapping3(fs_in.texCoords, wo);
  else
    texCoordsDisplaced = parallaxMapping1(fs_in.texCoords, wo);

  if (texCoordsDisplaced.x < 0 || texCoordsDisplaced.x > 1 ||
      texCoordsDisplaced.y < 0 || texCoordsDisplaced.y > 1)
    discard;
  vec3 n = texture(mapNormal, texCoordsDisplaced).rgb;
  vec3 diffuse = texture(texDiffuse, texCoordsDisplaced).rgb;
  n = normalize(TBN * (n * 2.0 - 1.0));
  // Ambient
  vec3 colorAmbient = diffuse * 0.05;

  // Diffuse
  float diff = max(0, dot(n, wi));
  vec3 colorDiffuse = diff * diffuse;

  // Specular
  float spec = 0;
  //// Blinn-Phong
  vec3 wHalf = normalize(wi + wo);
  spec = pow(max(0, dot(n, wHalf)), 64);
  vec3 colorSpecular = spec * vec3(0.3);

  fragColor = vec4(       //
      colorAmbient        //
          + colorDiffuse  //
          + colorSpecular //
      ,
      1.0);
}

vec2 parallaxMapping1(vec2 texCoord, vec3 viewDir) {
  float height = texture(mapDisplace, texCoord).r;
  vec2 p = viewDir.xy / viewDir.z * (height * heightScale);
  return texCoord - p;
}

vec2 parallaxMapping2(vec2 texCoord, vec3 viewDir) {
  float nLayers = mix(8, 32, max(0, dot(vec3(0, 0, 1), viewDir)));
  float stpDepth = 1.0 / nLayers;
  vec2 stpTexCoord = viewDir.xy / viewDir.z * heightScale / nLayers;
  float curLayerDepth = 0.0;
  vec2 curTexCoord = texCoord;
  float curMappingDepth = texture(mapDisplace, curTexCoord).r;
  while (curMappingDepth > curLayerDepth) {
    curTexCoord -= stpTexCoord;
    curLayerDepth += stpDepth;
    curMappingDepth = texture(mapDisplace, curTexCoord).r;
  }
  return curTexCoord;
}

vec2 parallaxMapping3(vec2 texCoord, vec3 viewDir) {
  float nLayers = mix(32, 8, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
  float stpDepth = 1.0 / nLayers;
  vec2 stpTexCoord = viewDir.xy / viewDir.z * heightScale / nLayers;

  float curLayerDepth = 0.0;
  vec2 curTexCoord = texCoord;
  float curMappingDepth = texture(mapDisplace, curTexCoord).r;

  while (curMappingDepth > curLayerDepth) {
    curTexCoord -= stpTexCoord;
    curLayerDepth += stpDepth;
    curMappingDepth = texture(mapDisplace, curTexCoord).r;
  }

  float depthAfter = abs(curMappingDepth - curLayerDepth);
  float depthBefore = abs(texture(mapDisplace, curTexCoord + stpTexCoord).r -
                          (curLayerDepth - stpDepth));
  return curTexCoord + depthAfter / (depthAfter + depthBefore) * stpTexCoord;
}