#version 330 core
out vec4 fragColor;

in VS_OUT {
  vec3 fragPos;
  vec3 normal;
  vec2 texCoords;
}
fs_in;

uniform sampler2D texDiffuse;
uniform samplerCube texShadow;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float far_plane;

float fragInLight(vec3 fragPosition) {
  vec3 lightToFrag = fragPosition - lightPos;
  float firstHit = texture(texShadow, lightToFrag).r;
  // [0, 1] to real distance.
  firstHit *= far_plane;
  float curHit = length(lightToFrag);
  float bias =
      max(0.0015 * (1.0 - dot(fs_in.normal, -normalize(lightToFrag))), 0.0002);

  float inLight = curHit - bias > firstHit ? 0.0 : 1.0;
  fragColor = vec4(vec3(firstHit / far_plane), 1.0);
  return inLight;
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

  float inLight = fragInLight(fs_in.fragPos);
  // fragColor = vec4(
  //     colorAmbient + colorDiffuse * inLight + colorSpecular * inLight, 1.0);
}