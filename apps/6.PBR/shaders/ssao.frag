#version 330
out float FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];
uniform mat4 projection;

// Tile scale.
const vec2 noiseScale = vec2(1024.0 / 4, 768.0 / 4);

void main() {
  vec3 fragPos = texture(gPosition, TexCoords).xyz;
  vec3 normal = normalize(texture(gNormal, TexCoords).rgb);
  vec3 randomVec = texture(texNoise, TexCoords * noiseScale).xyz;

  // Normal vector is in view-space during g-pass,
  // so this TBN is from tangent-space to view-space.
  vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
  vec3 bitangent = cross(normal, tangent);
  mat3 TBN = mat3(tangent, bitangent, normal);

  float occlusion = 0;
  const float bias = 0.025;
  const float radius = 0.5;
  for (int i = 0; i < 64; i++) {
    // Into view-space.
    vec3 samplePos = TBN * samples[i];
    samplePos = fragPos + samplePos * radius;

    // Into clip-space.
    vec4 offset = vec4(samplePos, 1.0);
    offset = projection * offset;
    // Perspective divide.
    offset.xyz /= offset.w;
    offset.xyz = offset.xyz * 0.5 + 0.5;

    // Sample a depth.
    float sampleDepth = texture(gPosition, offset.xy).z;
    float rangeCheck =
        smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
    occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
  }
  occlusion = 1.0 - (occlusion / 64);
  FragColor = occlusion;
}