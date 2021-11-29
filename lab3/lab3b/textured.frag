#version 150

out vec4 outColor;

in vec2 texCoord;
in vec3 exNormal;
in float height;
uniform sampler2D tex;

void main(void) {
  // Texture from disk
  vec4 t = texture(tex, texCoord);

  vec3 n = normalize(exNormal);
  float shade = n.y + n.z;

  outColor = t + (shade / 5);

  // outColor = vec4(texCoord.s, texCoord.t, 0, 1);
  //	outColor = vec4(n.x, n.y, n.z, 1);
  //	outColor = vec4(1) * shade;
}
 /* 
#define MAXHEIGHT 255

if(heightvalue >= 0 && heightvalue =< 128) {
// interpolate between (1.0f, 0.0f, 0.0f) and (0.0f, 1.0f, 0.0f)
green = heightvalue / 128.0f;
red = 1.0f - green;
blue = 0.0f;

} else if(heightvalue > 128 && heightvalue <= 255) {
// interpolate between (0.0f, 1.0f, 0.0f) and (0.0f, 0.0f, 1.0f)
red = 0.0f;
blue = (heightvalue - 127) / 128.0f;
green = 1.0f - blue;
}
*/
