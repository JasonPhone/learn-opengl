#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 model;

in VS_OUT {
  vec3 fragWorldPos;
  vec3 faceWorldNormal;
  vec2 texCoords;
  vec2 texWorldCoords;
}
gs_in[];
out GS_OUT {
  vec3 fragWorldPos;
  vec3 faceWorldNormal;
  vec2 texCoords;
}
gs_out;
out mat3 TBN;

void main() {
  // Calculate TBN.
  vec2 uv01 = gs_in[1].texWorldCoords - gs_in[0].texWorldCoords;
  vec2 uv02 = gs_in[2].texWorldCoords - gs_in[0].texWorldCoords;
  vec3 pos01 = gs_in[1].fragWorldPos - gs_in[0].fragWorldPos;
  vec3 pos02 = gs_in[2].fragWorldPos - gs_in[0].fragWorldPos;
  /**
   *  uv01.x * T + uv01.y * B = pos01
   *  uv02.x * T + uv02.y * B = pos02
   *              |
   *              V
   *  | uv01 |   | T |   | pos01 |
   *  |      | * |   | = |       |
   *  | uv02 |   | B |   | pos02 |
   *
   */
  float a = uv01.x, b = uv01.y, c = uv02.x, d = uv02.y;
  float admbcInv = 1.0 / (a * d - b * c);
  vec3 T, B;
  T.x = admbcInv * (d * pos01.x - b * pos02.x);
  T.y = admbcInv * (d * pos01.y - b * pos02.y);
  T.z = admbcInv * (d * pos01.z - b * pos02.z);
  B.x = admbcInv * (d * pos01.x - b * pos02.x);
  B.y = admbcInv * (d * pos01.y - b * pos02.y);
  B.z = admbcInv * (d * pos01.z - b * pos02.z);
  T = normalize(T);
  B = normalize(B);
  vec3 N = normalize(gs_in[0].faceWorldNormal);
  TBN = mat3(T, B, N);

  // vec3 edge1 = gs_in[1].fragWorldPos - gs_in[0].fragWorldPos;
  // vec3 edge2 = gs_in[2].fragWorldPos - gs_in[0].fragWorldPos;
  // vec2 deltaUV1 = gs_in[1].texWorldCoords - gs_in[0].texWorldCoords;
  // vec2 deltaUV2 = gs_in[2].texWorldCoords - gs_in[0].texWorldCoords;
  // float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
  // vec3 tangent1, bitangent1;
  // tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
  // tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
  // tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
  // bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
  // bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
  // bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
  // vec3 T = normalize(vec3(model * vec4(tangent1, 0.0)));
  // vec3 B = normalize(vec3(model * vec4(bitangent1, 0.0)));
  // vec3 N = normalize(gs_in[0].faceWorldNormal);
  // TBN = mat3(T, B, N);

  T = normalize(T - dot(T, N) * N);
  B = cross(N, T);
  TBN = mat3(T, B, N);

  // Emit three vertex.
  for (int i = 0; i < 3; i++) {
    gs_out.fragWorldPos = gs_in[i].fragWorldPos;
    gs_out.faceWorldNormal = gs_in[i].faceWorldNormal;
    gs_out.texCoords = gs_in[i].texCoords;

    gl_Position = gl_in[i].gl_Position;
    EmitVertex();
  }
  EndPrimitive();
}