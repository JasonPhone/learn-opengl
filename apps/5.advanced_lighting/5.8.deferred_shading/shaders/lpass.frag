#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffSpec;

struct Light {
  vec3 Position;
  vec3 Color;
  float aLinear;
  float aQuadratic;
};
const int N_LIGHTS = 32;
uniform Light lights[N_LIGHTS];
uniform vec3 viewPos;

void main() {
  // Get data from g-buffer.
  vec3 FragPos = texture(gPosition, TexCoords).rgb;
  vec3 Normal = texture(gNormal, TexCoords).rgb;
  vec3 Diffuse = texture(gDiffSpec, TexCoords).rgb;
  float Specular = texture(gDiffSpec, TexCoords).a;

  // Usual shading for every light.
  vec3 result = Diffuse * .1; // Ambient.
  vec3 wo = normalize(viewPos - FragPos);
  for (int i = 0; i < N_LIGHTS; i++) {
    // Diffuse.
    vec3 wi = normalize(lights[i].Position - FragPos);
    vec3 diffuse = max(0, dot(Normal, wi)) * Diffuse * lights[i].Color;
    // Specular.
    vec3 halfVec = normalize(wi + wo);
    float spec = pow(max(0, dot(Normal, halfVec)), 16);
    vec3 specular = lights[i].Color * spec * Specular;
    // Light attenuation.
    float dis = length(lights[i].Position - FragPos);
    float attenuation = 1.0 / (1.0 + lights[i].aLinear * dis +
                               lights[i].aQuadratic * dis * dis);

    diffuse *= attenuation;
    specular *= attenuation;
    result += diffuse + specular;
  }

  FragColor = vec4(result, 1.0);
}