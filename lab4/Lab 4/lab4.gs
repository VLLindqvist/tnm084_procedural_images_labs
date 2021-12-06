#version 410 core

layout(triangles) in;
// Use line_strip for visualization and triangle_strip for solids
layout(triangle_strip, max_vertices = 3) out;
// layout(line_strip, max_vertices = 6) out;
in vec2 teTexCoord[3];
in vec3 teNormal[3];
out vec2 gsTexCoord;
out vec3 gsNormal;
out vec3 gsPosition;
uniform sampler2D tex;

uniform mat4 projMatrix;
uniform mat4 mdlMatrix;
uniform mat4 camMatrix;

uniform float disp;
uniform int texon;

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

vec3 random3(vec3 st) {
  st = vec3(dot(st, vec3(127.1, 311.7, 543.21)), dot(st, vec3(269.5, 183.3, 355.23)), dot(st, vec3(846.34, 364.45, 123.65))); // Haphazard additional numbers by IR
  return -1.0 + 2.0 * fract(sin(st) * 43758.5453123);
}

// Gradient Noise by Inigo Quilez - iq/2013
// https://www.shadertoy.com/view/XdXGW8
// Trivially extended to 3D by Ingemar
float noise(vec3 st) {
  vec3 i = floor(st);
  vec3 f = fract(st);

  vec3 u = f * f * (3.0 - 2.0 * f);

  return mix(mix(mix(dot(random3(i + vec3(0.0, 0.0, 0.0)), f - vec3(0.0, 0.0, 0.0)), dot(random3(i + vec3(1.0, 0.0, 0.0)), f - vec3(1.0, 0.0, 0.0)), u.x),
                 mix(dot(random3(i + vec3(0.0, 1.0, 0.0)), f - vec3(0.0, 1.0, 0.0)), dot(random3(i + vec3(1.0, 1.0, 0.0)), f - vec3(1.0, 1.0, 0.0)), u.x), u.y),

             mix(mix(dot(random3(i + vec3(0.0, 0.0, 1.0)), f - vec3(0.0, 0.0, 1.0)), dot(random3(i + vec3(1.0, 0.0, 1.0)), f - vec3(1.0, 0.0, 1.0)), u.x),
                 mix(dot(random3(i + vec3(0.0, 1.0, 1.0)), f - vec3(0.0, 1.0, 1.0)), dot(random3(i + vec3(1.0, 1.0, 1.0)), f - vec3(1.0, 1.0, 1.0)), u.x), u.y),
             u.z

  );
}

float fbm(vec3 st, int octaves, float initialAmp) {
  // Initial values
  float value = 0.0;
  float amplitude = initialAmp;

  // Loop over octaves
  for (int i = 0; i < octaves; i++) {
    value += amplitude * noise(st);
    st *= 2.0;
    amplitude /= 2;
  }

  return value;
}

vec3 cartesianToSpherical(vec3 cartesian) {
  vec3 spherical;
  spherical.x = sqrt(cartesian.x * cartesian.x + cartesian.y * cartesian.y + cartesian.z * cartesian.z);
  spherical.y = atan(sqrt(cartesian.x * cartesian.x + cartesian.y * cartesian.y), cartesian.z);
  spherical.z = atan(cartesian.y, cartesian.x);
  return spherical;
}

vec3 sphericalToCartesian(vec3 spherical) {
  vec3 cartesian;
  cartesian.x = spherical.x * sin(spherical.y) * cos(spherical.z);
  cartesian.y = spherical.x * sin(spherical.y) * sin(spherical.z);
  cartesian.z = spherical.x * cos(spherical.y);
  return cartesian;
}

void computeVertex(int nr) {
  vec3 p = vec3(gl_in[nr].gl_Position);

  // Add interesting code here
  p = normalize(p);
  gl_Position = projMatrix * camMatrix * mdlMatrix * vec4(p, 1.0);

  // Convert to Spherical coordinates
  vec3 pSpherical = cartesianToSpherical(vec3(p));

  // Find points around p on the sphere
  vec3 v1Sphere = vec3(pSpherical.x, pSpherical.y + .02, pSpherical.z + .02);
  vec3 v2Sphere = vec3(pSpherical.x, pSpherical.y - .02, pSpherical.z - .02);
  vec3 v3Sphere = vec3(pSpherical.x, pSpherical.y + .02, pSpherical.z - .02);

  // Convert back to cartesian and create vectors
  vec3 v1 = sphericalToCartesian(v1Sphere) - vec3(p);
  vec3 v2 = sphericalToCartesian(v2Sphere) - vec3(p);
  vec3 v3 = sphericalToCartesian(v3Sphere) - vec3(p);

  // Move small step in vector directions
  vec3 p1 = v1 * .1;
  vec3 p2 = v2 * .1;
  vec3 p3 = v3 * .1;

  // Create plane
  vec3 s1 = p2 - p1;
  vec3 s2 = p3 - p1;

  vec3 n = teNormal[nr];
  gsNormal = mat3(camMatrix * mdlMatrix) * n;
  gsNormal += vec3(gsNormal * fbm(6.0 * normalize(cross(s1, s2)), 5, .4));

  gsTexCoord = teTexCoord[0];

  /* ======= noise (Task 3) ======= */
  // gl_Position += vec4(normalize(vec3(gl_Position)) * noise(10.0 * p) * 0.4, 1.0);
  /* ======== fbm (Task 4) ======== */
  gl_Position += vec4(normalize(vec3(gl_Position)) * fbm(6.0 * p, 5, .4), 1.0);
  gsPosition = gl_Position.xyz;

  EmitVertex();
}

void main() {
  for (int idx = 0; idx < 3; ++idx) {
    computeVertex(idx);
  }
}
