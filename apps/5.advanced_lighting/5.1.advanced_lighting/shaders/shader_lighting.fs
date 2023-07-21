#version 330 core
out vec4 fragColor;

in VS_OUT {
  vec3 fragPos;
  vec3 normal;
  vec2 texCoords;
}
fs_in;

uniform sampler2D texture1;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool blinn;

void main() {
  vec3 color = texture(texture1, fs_in.texCoords).rgb;
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
  if (blinn) {
    // Blinn-Phong
    vec3 wHalf = normalize(wi + wo);
    spec = pow(max(0, dot(n, wHalf)), 32);

    // vec3 colorSpecular = spec * vec3(0.3);
    // fragColor = vec4(colorAmbient + colorDiffuse + colorSpecular, 1.0);
  } else {
    // Phong
    vec3 wReflect = reflect(-wi, n);
    spec = pow(max(0, dot(wo, wReflect)), 8);

    // vec3 colorSpecular = spec * vec3(0.3);
    // fragColor = vec4(colorAmbient + colorDiffuse + colorSpecular, 1.0);
  }
  vec3 colorSpecular = spec * vec3(0.3);
  fragColor = vec4(colorAmbient + colorDiffuse + colorSpecular, 1.0);
}