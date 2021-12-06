#version 410 core

uniform float time;

out vec4 out_Color;
in vec2 gsTexCoord;
in vec3 gsNormal;
in vec3 gsPosition;

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

void main(void) {
  vec3 shade = vec3(normalize(gsNormal).z); // Fake light
  shade = cartesianToSpherical(shade);
  // shade.y += time / 10;
  shade = sphericalToCartesian(shade);

  // out_Color = vec4(gsTexCoord.s, gsTexCoord.t, 0.0, 1.0);
  // out_Color = vec4(gsNormal.x, gsNormal.y, gsNormal.z, 1.0);
  out_Color = vec4(shade, 1.0);
}
