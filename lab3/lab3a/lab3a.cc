// Fractal tree generation

#ifdef __APPLE__
#include <OpenGL/gl3.h>
// uses framework Cocoa
#endif
#include "GL_utilities.h"
#include "LittleOBJLoader.h"
#include "LoadTGA.h"
#include "MicroGlut.h"
#include "VectorUtils3.h"
#include "glugg.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

// uses framework OpenGL
// uses framework Cocoa

void MakeCylinderAlt(int aSlices, float height, float topwidth, float bottomwidth) {
  gluggMode(GLUGG_TRIANGLE_STRIP);
  vec3 top = SetVector(0, height, 0);
  vec3 center = SetVector(0, 0, 0);
  vec3 bn = SetVector(0, -1, 0); // Bottom normal
  vec3 tn = SetVector(0, 1, 0);  // Top normal

  for (float a = 0.0; a < 2.0 * M_PI + 0.0001; a += 2.0 * M_PI / aSlices) {
    float a1 = a;

    vec3 p1 = SetVector(topwidth * cos(a1), height, topwidth * sin(a1));
    vec3 p2 = SetVector(bottomwidth * cos(a1), 0, bottomwidth * sin(a1));
    vec3 pn = SetVector(cos(a1), 0, sin(a1));

    // Done making points and normals. Now create polygons!
    gluggNormalv(pn);
    gluggTexCoord(height, a1 / M_PI);
    gluggVertexv(p2);
    gluggTexCoord(0, a1 / M_PI);
    gluggVertexv(p1);
  }

  // Then walk around the top and bottom with fans
  gluggMode(GLUGG_TRIANGLE_FAN);
  gluggNormalv(bn);
  gluggVertexv(center);
  // Walk around edge
  for (float a = 0.0; a <= 2.0 * M_PI + 0.001; a += 2.0 * M_PI / aSlices) {
    vec3 p = SetVector(bottomwidth * cos(a), 0, bottomwidth * sin(a));
    gluggVertexv(p);
  }
  // Walk around edge
  gluggMode(GLUGG_TRIANGLE_FAN); // Reset to new fan
  gluggNormalv(tn);
  gluggVertexv(top);
  for (float a = 2.0 * M_PI; a >= -0.001; a -= 2.0 * M_PI / aSlices) {
    vec3 p = SetVector(topwidth * cos(a), height, topwidth * sin(a));
    gluggVertexv(p);
  }
}

mat4 projectionMatrix;

Model *floormodel;
GLuint grasstex, barktex;

// Reference to shader programs
GLuint phongShader, texShader;

// Floor quad
GLfloat vertices2[] = {-20.5, 0.0, -20.5, 20.5, 0.0, -20.5, 20.5, 0.0, 20.5, -20.5, 0.0, 20.5};
GLfloat normals2[] = {0, 1.0, 0, 0, 1.0, 0, 0, 1.0, 0, 0, 1.0, 0};
GLfloat texcoord2[] = {50.0f, 50.0f, 0.0f, 50.0f, 0.0f, 0.0f, 50.0f, 0.0f};
GLuint indices2[] = {0, 3, 2, 0, 2, 1};

// THIS IS WHERE YOUR WORK GOES!

float randRange(int min, int max) { return rand() % (max + 1 - min) + min; }
double randRange(double min, double max) {
  int _min = floor(min * 10);
  int _max = floor(max * 10);

  return (rand() % (_max + 1 - _min) + _min) / (double)10;
}

void generateTree(int depth, int maxDepth) {
  MakeCylinderAlt(20, 2.0, 0.03, 0.15);

  for (int i = 0; i < randRange(3, depth + 3); ++i) {
    gluggPushMatrix();

    double scale = ((maxDepth - depth) * randRange(0.8, 1.0) + depth) / maxDepth;
    scale = 1.6 - scale;

    gluggScale(scale, scale * 1.1, scale);
    gluggTranslate(.0, randRange(0, 10) < 5 ? randRange(1.4, 1.7) : randRange(.8, 1.1), .0);
    gluggRotate(randRange(.8, 1.5) * (randRange(0, 10) < 5 ? -1.0 : 1.0), 1.0, randRange(0, 10) < 5 ? -1.0 : 1.0, randRange(0, 10) < 5 ? -1.0 : 1.0);

    if (depth == maxDepth) {
      MakeCylinderAlt(20, .8, 0.15, 0.15);
    } else {
      generateTree(depth + 1, maxDepth);
    }

    gluggPopMatrix();
  }

  return;
}

GLuint MakeTree(int *count, GLuint program) {
  gluggSetPositionName(strdup("inPosition"));
  gluggSetNormalName(strdup("inNormal"));
  gluggSetTexCoordName(strdup("inTexCoord"));

  gluggBegin(GLUGG_TRIANGLES);
  // Between gluggBegin and gluggEnd, call MakeCylinderAlt plus glugg transformations
  // to create a tree.

  generateTree(1, 8);

  return gluggEnd(count, program, 0);
}

GLuint tree;
int treecount;

void reshape(int w, int h) {
  glViewport(0, 0, w, h);

  // Set the clipping volume
  projectionMatrix = perspective(45, 1.0f * w / h, 1, 1000);
  glUseProgram(phongShader);
  glUniformMatrix4fv(glGetUniformLocation(phongShader, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
  glUseProgram(texShader);
  glUniformMatrix4fv(glGetUniformLocation(texShader, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
}

void init(void) {
  // GL inits
  glClearColor(0.2, 0.2, 0.5, 0);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  printError("GL inits");

  projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 300.0);

  // Load and compile shader
  phongShader = loadShaders("phong.vert", "phong.frag");
  texShader = loadShaders("textured.vert", "textured.frag");
  printError("init shader");

  // Upload geometry to the GPU:
  floormodel = LoadDataToModel((vec3 *)vertices2, (vec3 *)normals2, (vec2 *)texcoord2, NULL, indices2, 4, 6);

  // Important! The shader we upload to must be active!
  glUseProgram(phongShader);
  glUniformMatrix4fv(glGetUniformLocation(phongShader, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
  glUseProgram(texShader);
  glUniformMatrix4fv(glGetUniformLocation(texShader, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);

  glUniform1i(glGetUniformLocation(texShader, "tex"), 0); // Texture unit 0

  LoadTGATextureSimple(strdup("grass.tga"), &grasstex);
  glBindTexture(GL_TEXTURE_2D, grasstex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  LoadTGATextureSimple(strdup("bark2.tga"), &barktex);

  tree = MakeTree(&treecount, texShader);

  printError("init arrays");
}

GLfloat a = 0.0;
vec3 campos = {0, 1.5, 10};
vec3 forward = {0, 0, -4};
vec3 up = {0, 1, 0};

void display(void) {
  printError(strdup("pre display"));

  // clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  mat4 worldToView, m; // m1, m2, m, tr, scale;

  if (glutKeyIsDown('a'))
    forward = MultMat3Vec3(mat4tomat3(Ry(0.05)), forward);
  if (glutKeyIsDown('d'))
    forward = MultMat3Vec3(mat4tomat3(Ry(-0.05)), forward);
  if (glutKeyIsDown('w'))
    campos = VectorAdd(campos, ScalarMult(forward, 0.1));
  if (glutKeyIsDown('s'))
    campos = VectorSub(campos, ScalarMult(forward, 0.1));
  if (glutKeyIsDown('q')) {
    vec3 side = CrossProduct(forward, SetVector(0, 1, 0));
    campos = VectorSub(campos, ScalarMult(side, 0.05));
  }
  if (glutKeyIsDown('e')) {
    vec3 side = CrossProduct(forward, SetVector(0, 1, 0));
    campos = VectorAdd(campos, ScalarMult(side, 0.05));
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

  a += 0.1;

  glBindTexture(GL_TEXTURE_2D, grasstex);
  // Floor
  glUseProgram(texShader);
  m = worldToView;
  glUniformMatrix4fv(glGetUniformLocation(texShader, "modelviewMatrix"), 1, GL_TRUE, m.m);
  DrawModel(floormodel, texShader, "inPosition", "inNormal", "inTexCoord");

  // Draw the tree, as defined on MakeTree
  glBindTexture(GL_TEXTURE_2D, barktex);
  glUseProgram(texShader);
  m = Mult(worldToView, T(0, 0, 0));
  glUniformMatrix4fv(glGetUniformLocation(texShader, "modelviewMatrix"), 1, GL_TRUE, m.m);
  glBindVertexArray(tree); // Select VAO
  glDrawArrays(GL_TRIANGLES, 0, treecount);

  printError("display");

  glutSwapBuffers();
}

void keys(unsigned char key, int x, int y) {
  switch (key) {
  case ' ':
    forward.y = 0;
    forward = ScalarMult(normalize(forward), 4.0);
    break;
  }
}

int main(int argc, char *argv[]) {
  glutInit(&argc, argv);
  glutInitContextVersion(3, 2);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowPosition(100, 100);
  glutInitWindowSize(640, 360);
  glutCreateWindow(strdup("Fractal tree - Lab3a"));
  glutRepeatingTimer(20);
  glutDisplayFunc(display);
  glutKeyboardFunc(keys);
  glutReshapeFunc(reshape);
  init();
  glutMainLoop();
}
