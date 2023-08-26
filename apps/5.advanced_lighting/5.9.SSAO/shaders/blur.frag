#version 330
out float FragColor;

in vec2 TexCoords;
uniform sampler2D texSSAO;

void main() {
  vec2 texelSize = 1.0 / vec2(textureSize(texSSAO, 0));
  float result = 0.0;
  int kernelRadius = 2;
  for (int x = -kernelRadius; x < kernelRadius; ++x) {
    for (int y = -kernelRadius; y < kernelRadius; ++y) {
      vec2 offset = vec2(float(x), float(y)) * texelSize;
      result += texture(texSSAO, TexCoords + offset).r;
    }
  }
  FragColor = result / (4.0 * kernelRadius * kernelRadius);
}