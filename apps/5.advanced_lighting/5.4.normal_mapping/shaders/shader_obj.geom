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
out vec3 fT, fB;

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
  fT = normalize(T);
  fB = normalize(B);

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