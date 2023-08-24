#version 330 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

in VS_OUT {
  vec3 FragPos;
  vec3 Normal;
  vec2 TexCoords;
}
fs_in;

struct Light {
  vec3 Position;
  vec3 Color;
};

uniform Light lights[16];
uniform sampler2D diffuseTexture;
uniform vec3 viewPos;

void main() {
  vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
  vec3 normal = normalize(fs_in.Normal);
  // ambient
  vec3 ambient = 0.0 * color;
  // lighting
  vec3 lighting = vec3(0.0);
  for (int i = 0; i < 16; i++) {
    // diffuse
    vec3 lightDir = normalize(lights[i].Position - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = lights[i].Color * diff * color;
    vec3 result = diffuse;
    // attenuation (use quadratic as we have gamma correction)
    float dis = length(fs_in.FragPos - lights[i].Position);
    result *= 1.0 / (dis * dis);
    lighting += result;
  }
  FragColor = vec4(ambient + lighting, 1.0);

  float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
  if (brightness > 1.0)
    BrightColor = vec4(FragColor.rgb, 1.0);
  else
    BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}