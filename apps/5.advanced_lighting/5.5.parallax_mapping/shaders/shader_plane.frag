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

void main() {
  vec3 color = texture(texDiffuse, fs_in.texCoords).rgb;
  // Use interpolated normal.
  vec3 N = fs_in.fragWorldNormal;
  vec3 T = normalize(fT - dot(fT, N) * N);
  vec3 B = cross(N, T);
  mat3 TBN = mat3(T, B, N);
  // Ambient
  vec3 colorAmbient = color * 0.05;

  // Diffuse
  vec3 wi = normalize(lightPos - fs_in.fragWorldPos);
  vec3 n = texture(mapNormal, fs_in.texCoords).rgb;
  n = normalize(TBN * (n * 2.0 - 1.0));
  float diff = max(0, dot(n, wi));
  vec3 colorDiffuse = diff * color;

  // Specular
  vec3 wo = normalize(viewPos - fs_in.fragWorldPos);
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