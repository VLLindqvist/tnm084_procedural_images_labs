// Lab 1 fragment shader
// Output either the generated texture from CPU or generate a similar pattern.
// Functions for gradient and cellular noise included. Not necessarily the best ones
// and not the same as the CPU code but they should be OK for the lab.

#version 150

out vec4 out_Color;
in vec2 texCoord;
uniform sampler2D tex;

uniform int displayGPUversion;
uniform float brickWidth;
uniform float brickHeight;
uniform float mortarThickness;
uniform int textureSize;
uniform float noiseWeight;
uniform int time;

vec2 random2(vec2 st) {
  st = vec2(dot(st, vec2(127.1, 311.7)), dot(st, vec2(269.5, 183.3)));
  return -1.0 + 2.0 * fract(sin(st) * 43758.5453123);
}

// Gradient Noise by Inigo Quilez - iq/2013
// https://www.shadertoy.com/view/XdXGW8
float noise(vec2 st) {
  vec2 i = floor(st);
  vec2 f = fract(st);

  vec2 u = f * f * (3.0 - 2.0 * f);

  return mix(mix(dot(random2(i + vec2(0.0, 0.0)), f - vec2(0.0, 0.0)), dot(random2(i + vec2(1.0, 0.0)), f - vec2(1.0, 0.0)), u.x),
             mix(dot(random2(i + vec2(0.0, 1.0)), f - vec2(0.0, 1.0)), dot(random2(i + vec2(1.0, 1.0)), f - vec2(1.0, 1.0)), u.x), u.y);
}

// Voronoise Created by inigo quilez - iq/2013
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
// http://iquilezles.org/www/articles/voronoise/voronoise.htm
vec3 hash3(vec2 p) {
  vec3 q = vec3(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)), dot(p, vec2(419.2, 371.9)));
  return fract(sin(q) * 43758.5453);
}

float iqnoise(in vec2 x, float u, float v) {
  vec2 p = floor(x);
  vec2 f = fract(x);

  float k = 1.0 + 63.0 * pow(1.0 - v, 4.0);

  float va = 0.0;
  float wt = 0.0;
  for (int j = -2; j <= 2; j++) {
    for (int i = -2; i <= 2; i++) {
      vec2 g = vec2(float(i), float(j));
      vec3 o = hash3(p + g) * vec3(u, u, 1.0);
      vec2 r = g - f + o.xy;
      float d = dot(r, r);
      float ww = pow(1.0 - smoothstep(0.0, 1.414, sqrt(d)), k);
      va += o.z * ww;
      wt += ww;
    }
  }

  return va / wt;
}

float getBrickXFrac(float offset) { return fract(((texCoord.s * textureSize) + offset) / brickWidth); }

void gpuBrickTexture(void) {
  float fracYBrick = fract((texCoord.t * textureSize) / brickHeight);

  int rowNumber = int(floor((texCoord.t * textureSize)) / brickHeight);
  int amountOfRows = int(floor(textureSize / brickHeight));
  bool evenRow = rowNumber % 2 != 0;
  bool brickInX = false;

  int animationMult = (rowNumber == 0 || (amountOfRows - rowNumber) == 0) ? 30 : (rowNumber > (amountOfRows / 2) ? rowNumber % amountOfRows : (amountOfRows - rowNumber) % amountOfRows);

  if (evenRow) {
    brickInX = getBrickXFrac((time / -60) * 0.1 * animationMult) < brickWidth / (brickWidth + mortarThickness);
  } else if (getBrickXFrac((time / -60) * 0.1 * animationMult) < (brickWidth / 2) / (brickWidth + mortarThickness)) {
    brickInX = true;
  } else if (getBrickXFrac((time / -60) * 0.1 * animationMult) > ((brickWidth / 2) + mortarThickness) / (brickWidth + mortarThickness)) {
    brickInX = true;
  }

  if (brickInX && fracYBrick < (brickHeight / (brickHeight + mortarThickness))) {
    out_Color = vec4(1.0, 100.0 / 255.0, 50.0 / 255.0, 1.0) - (vec4(hash3(texCoord) * noise(texCoord * 0.2) * 3.0, 1.0) * 0.3);
  } else {
    out_Color = vec4(80.0 / 255.0, 80.0 / 255.0, 80.0 / 255.0, 1.0) - (vec4(hash3(texCoord * 10), 1.0) * 0.4);
  }
}

void main(void) {
  if (displayGPUversion == 1) {
    gpuBrickTexture();
  } else
    out_Color = texture(tex, texCoord);
}
