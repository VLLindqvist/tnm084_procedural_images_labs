// Lab 3b
// Terrain generation

// Current contents:
// Terrain being generated on CPU (MakeTerrain). Simple terrain as example: Flat surface with a bump.
// Note the constants kTerrainSize and kPolySize for changing the number of vertices and polygon size!

// Things to do:
// Generate a terrain on CPU
// Generate a terrain on GPU, in the vertex shader.
// Generate textures for the surface (fragment shader).

// If you want to use Perlin noise, use the code from Lab 1.

#ifdef __APPLE__
#include "MicroGlut.h"
#include <OpenGL/gl3.h>
// linking hint for Lightweight IDE
// uses framework Cocoa
#endif
#include "GL_utilities.h"
#include "LittleOBJLoader.h"
#include "LoadTGA.h"
#include "VectorUtils3.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <utility>
#include <vector>

mat4 projectionMatrix;
Model *floormodel;
GLuint grasstex;

// Reference to shader programs
GLuint phongShader, texShader;

#define kTerrainSize 32 // (2^n)+1 => n=5
#define kPolySize 1.0

// Terrain data. To be intialized in MakeTerrain or in the shader
vec3 vertices[kTerrainSize * kTerrainSize];
vec2 texCoords[kTerrainSize * kTerrainSize];
vec3 normals[kTerrainSize * kTerrainSize];
GLuint indices[(kTerrainSize - 1) * (kTerrainSize - 1) * 3 * 2];

// These are considered unsafe, but with most C code, write with caution.
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

int randomRange(int range) { return (rand() % (range * 2)) - range; }

void squareStep(int x, int z, int reach) {
  int count = 0;
  float avg = 0.0f;

  if (x - reach >= 0 && z - reach >= 0) {
    avg += vertices[(z - reach) * kTerrainSize + (x - reach)].y;
    count++;
  }

  if (x - reach >= 0 && z + reach < kTerrainSize) {
    avg += vertices[(z + reach) * kTerrainSize + (x - reach)].y;
    count++;
  }

  if (x + reach < kTerrainSize && z - reach >= 0) {
    avg += vertices[(z - reach) * kTerrainSize + (x + reach)].y;
    count++;
  }

  if (x + reach < kTerrainSize && z + reach < kTerrainSize) {
    avg += vertices[(z + reach) * kTerrainSize + (x + reach)].y;
    count++;
  }

  avg += randomRange(reach) * 1.0f;
  avg /= count;
  const size_t idx = z * kTerrainSize + x;
  vertices[idx] = SetVec3(x * kPolySize, avg, z * kPolySize);
  texCoords[idx] = SetVec2(x, z);
  normals[idx] = SetVec3(0, 1, 0);
}

void diamondStep(int x, int z, int reach) {
  int count = 0;
  float avg = 0.0f;

  if (x - reach >= 0) {
    avg += vertices[z * kTerrainSize + (x - reach)].y;
    count++;
  }

  if (x + reach < kTerrainSize) {
    avg += vertices[z * kTerrainSize + (x + reach)].y;
    count++;
  }

  if (z - reach > 0) {
    avg += vertices[(z - reach) * kTerrainSize + x].y;
    count++;
  }

  if (z + reach <= kTerrainSize) {
    avg += vertices[(z + reach) * kTerrainSize + x].y;
    count++;
  }

  avg += randomRange(reach) * 1.0f;
  avg /= count;
  const size_t idx = z * kTerrainSize + x;
  vertices[idx] = SetVec3(x * kPolySize, avg, z * kPolySize);
  texCoords[idx] = SetVec2(x, z);
  normals[idx] = SetVec3(0, 1, 0);
}

void diamondSquare(int size) {
  int half = size / 2;
  if (half < 1)
    return;

  // square steps
  for (int z = half; z < kTerrainSize; z += size)
    for (int x = half; x < kTerrainSize; x += size)
      squareStep(x % kTerrainSize, z % kTerrainSize, half);

  // diamond steps
  int col = 0;
  for (int x = 0; x < kTerrainSize; x += half) {
    col++;

    // If this is an odd column.
    if (col % 2 == 1) {
      for (int z = half; z < kTerrainSize; z += size) {
        diamondStep(x % kTerrainSize, z % kTerrainSize, half);
      }
    } else {
      for (int z = 0; z < kTerrainSize; z += size) {
        diamondStep(x % kTerrainSize, z % kTerrainSize, half);
      }
    }
  }

  diamondSquare(size / 2);
}

void MakeTerrain() {
  // // TO DO: This is where your terrain generation goes if on CPU.
  diamondSquare(kTerrainSize);
  // std::vector<std::pair<std::string, int>> normalsPerIndex;

  // Make indices
  // You don't need to change this.
  for (int x = 0; x < kTerrainSize - 1; x++)
    for (int z = 0; z < kTerrainSize - 1; z++) {
      // Quad count
      int q = (z * (kTerrainSize - 1)) + (x);
      const int polyonCount = q * 2;
      // Indices
      indices[polyonCount * 3] = x + z * kTerrainSize; // top left
      indices[polyonCount * 3 + 1] = x + 1 + z * kTerrainSize;
      indices[polyonCount * 3 + 2] = x + (z + 1) * kTerrainSize;
      indices[polyonCount * 3 + 3] = x + 1 + z * kTerrainSize;
      indices[polyonCount * 3 + 4] = x + 1 + (z + 1) * kTerrainSize;
      indices[polyonCount * 3 + 5] = x + (z + 1) * kTerrainSize;
    }

  // Make normal vectors
  // TO DO: This is where you calculate normal vectors
  for (int t = 0; t < std::size(indices) / 3; ++t) {
    unsigned int t1 = indices[t*3+0]; 
    unsigned int t2 = indices[t*3+1];
    unsigned int t3 = indices[t*3+2];
    const vec3& a = vertices[t1];
    const vec3& b = vertices[t2];
    const vec3& c = vertices[t3];
    vec3 u = b - a;
    vec3 v = c - a;
    vec3 n = cross(u, v);
    normals[t1] += n;
    normals[t2] += n;
    normals[t3] += n;
  }

  for (int v = 0; v < std::size(vertices); ++v)
    normals[v] = normalize(normals[v]);
}

void init(void) {
  // GL inits
  glClearColor(0.2, 0.2, 0.5, 0);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glDisable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  printError("GL inits");

  projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 300.0);

  // Load and compile shader
  texShader = loadShaders("textured.vert", "textured.frag");
  printError("init shader");

  // Upload geometry to the GPU:
  MakeTerrain();
  floormodel = LoadDataToModel(vertices, normals, texCoords, NULL, indices, kTerrainSize * kTerrainSize, (kTerrainSize - 1) * (kTerrainSize - 1) * 2 * 3);

  printError("LoadDataToModel");

  // Important! The shader we upload to must be active!
  glUseProgram(texShader);
  glUniformMatrix4fv(glGetUniformLocation(texShader, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);

  glUniform1i(glGetUniformLocation(texShader, "tex"), 0); // Texture unit 0

  // char *textureName = "grass.tga";
  LoadTGATextureSimple(strdup("grass.tga"), &grasstex);
  glBindTexture(GL_TEXTURE_2D, grasstex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  printError("init arrays");
}

vec3 campos = {kTerrainSize * kPolySize / 4, 1.5, kTerrainSize *kPolySize / 4};
vec3 forward = {8, 0, 8};
vec3 up = {0, 1, 0};

void display(void) {
  printError("pre display");

  // clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  mat4 worldToView, m;

  if (glutKeyIsDown('a'))
    forward = MultMat3Vec3(mat4tomat3(Ry(0.03)), forward);
  if (glutKeyIsDown('d'))
    forward = MultMat3Vec3(mat4tomat3(Ry(-0.03)), forward);
  if (glutKeyIsDown('w'))
    campos = VectorAdd(campos, ScalarMult(forward, 0.01));
  if (glutKeyIsDown('s'))
    campos = VectorSub(campos, ScalarMult(forward, 0.01));
  if (glutKeyIsDown('q')) {
    vec3 side = CrossProduct(forward, SetVector(0, 1, 0));
    campos = VectorSub(campos, ScalarMult(side, 0.01));
  }
  if (glutKeyIsDown('e')) {
    vec3 side = CrossProduct(forward, SetVector(0, 1, 0));
    campos = VectorAdd(campos, ScalarMult(side, 0.01));
  }

  // Move up/down
  if (glutKeyIsDown('z'))
    campos = VectorAdd(campos, ScalarMult(SetVector(0, 1, 0), 0.01));
  if (glutKeyIsDown('c'))
    campos = VectorSub(campos, ScalarMult(SetVector(0, 1, 0), 0.01));

  // NOTE: Looking up and down is done by making a side vector and rotation around arbitrary axis!
  if (glutKeyIsDown('+')) {
    vec3 side = CrossProduct(forward, SetVector(0, 1, 0));
    mat4 m = ArbRotate(side, 0.05);
    forward = MultMat3Vec3(mat4tomat3(m), forward);
  }
  if (glutKeyIsDown('-')) {
    vec3 side = CrossProduct(forward, SetVector(0, 1, 0));
    mat4 m = ArbRotate(side, -0.05);
    forward = MultMat3Vec3(mat4tomat3(m), forward);
  }

  worldToView = lookAtv(campos, VectorAdd(campos, forward), up);

  glBindTexture(GL_TEXTURE_2D, grasstex); // The texture is not used but provided as example
  // Floor
  glUseProgram(texShader);
  m = worldToView;
  glUniformMatrix4fv(glGetUniformLocation(texShader, "modelviewMatrix"), 1, GL_TRUE, m.m);
  DrawModel(floormodel, texShader, "inPosition", "inNormal", "inTexCoord");

  printError("display");

  glutSwapBuffers();
}

void keys(unsigned char key, int x, int y) {}

int main(int argc, char *argv[]) {
  glutInit(&argc, argv);
  glutInitContextVersion(3, 2);
  // char *windowTitle = "Lab 3b";
  glutCreateWindow(strdup("Lab 3b"));
  glutRepeatingTimer(20);
  glutDisplayFunc(display);
  glutKeyboardFunc(keys);
  init();
  glutMainLoop();
}
