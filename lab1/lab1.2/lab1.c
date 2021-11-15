// TNM084 Lab 1
// Procedural textures
// Draws concentric circles on CPU and GPU
// Make something more interesting!

// For the CPU part, you should primarily work in maketexture() below.

#ifdef __APPLE__
#include <OpenGL/gl3.h>
// uses framework Cocoa
#endif
#include "GL_utilities.h"
#include "MicroGlut.h"
#include <math.h>
#include <stdio.h>

// Lab 1 specific
// Code for you to access to make interesting patterns.
// This is for CPU. For GPU, I provide sepatrate files
// with functions that you can add to your fragment shaders.
#include "cellular.h"
#include "noise1234.h"
#include "simplexnoise1234.h"

#define kTextureSize 512
GLubyte ptex[kTextureSize][kTextureSize][3];
/* This is the frequency */
// const float ringDensity = 30.0;
/* ===================== */
const double brickWidth = 40.0;
const double brickHeight = 17.0;
const double mortarThickness = 4.0;

// Brick pattern
void maketexture() {
  for (int x = 0; x < kTextureSize; x++) {
    for (int y = 0; y < kTextureSize; y++) {
      double fracXBrick, intPart, fracYBrick;
      fracXBrick = modf((double)x / brickWidth, &intPart);
      fracYBrick = modf((double)y / brickHeight, &intPart);

      unsigned evenRow = (int)(y / brickHeight) % 2 != 0;
      unsigned short brickInX = 0;

      if (evenRow) {
        brickInX = fracXBrick < (brickWidth / (brickWidth + mortarThickness));
      } else if (fracXBrick < (brickWidth / 2) / (brickWidth + mortarThickness)) {
        brickInX = 1;
      } else if (fracXBrick > ((brickWidth / 2) + mortarThickness) / (brickWidth + mortarThickness)) {
        brickInX = 1;
      }

      if (brickInX && fracYBrick < (brickHeight / (brickHeight + mortarThickness))) {
        ptex[y][x][0] = 255.0;
        ptex[y][x][1] = 100.0;
        ptex[y][x][2] = 50.0;
      } else {
        ptex[y][x][0] = 80.0;
        ptex[y][x][1] = 80.0;
        ptex[y][x][2] = 80.0;
      }
    }
  }
}

// Globals
// Data would normally be read from files
GLfloat vertices[] = {-1.0f, -1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f};
GLfloat texCoords[] = {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f};

// vertex array object
unsigned int vertexArrayObjID;
// Texture reference
GLuint texid;
// Switch between CPU and shader generation
int displayGPUversion = 0;
// Reference to shader program
GLuint program;

void init(void) {
  // two vertex buffer objects, used for uploading the
  unsigned int vertexBufferObjID;
  unsigned int texBufferObjID;

  // GL inits
  glClearColor(0.2, 0.2, 0.5, 0);
  glEnable(GL_DEPTH_TEST);
  printError("GL inits");

  // Load and compile shader
  program = loadShaders("lab1.vert", "lab1.frag");
  glUseProgram(program);
  printError("init shader");

  // Upload gemoetry to the GPU:

  // Allocate and activate Vertex Array Object
  glGenVertexArrays(1, &vertexArrayObjID);
  glBindVertexArray(vertexArrayObjID);
  // Allocate Vertex Buffer Objects
  glGenBuffers(1, &vertexBufferObjID);
  glGenBuffers(1, &texBufferObjID);

  // VBO for vertex data
  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID);
  glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(glGetAttribLocation(program, "in_Position"), 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(glGetAttribLocation(program, "in_Position"));

  // VBO for texture
  glBindBuffer(GL_ARRAY_BUFFER, texBufferObjID);
  glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), texCoords, GL_STATIC_DRAW);
  glVertexAttribPointer(glGetAttribLocation(program, "in_TexCoord"), 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(glGetAttribLocation(program, "in_TexCoord"));

  // Texture unit
  glUniform1i(glGetUniformLocation(program, "tex"), 0); // Texture unit 0

  // Constants common to CPU and GPU
  glUniform1i(glGetUniformLocation(program, "displayGPUversion"),
              0); // shader generation off
  glUniform1f(glGetUniformLocation(program, "ringDensity"), 1.0);

  maketexture();

  // Upload texture
  glGenTextures(1, &texid); // texture id
  glBindTexture(GL_TEXTURE_2D, texid);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, kTextureSize, kTextureSize, 0, GL_RGB, GL_UNSIGNED_BYTE, ptex);

  // End of upload of geometry
  printError("init arrays");
}

// Switch on any key
void key(unsigned char key, int x, int y) {
  displayGPUversion = !displayGPUversion;
  glUniform1i(glGetUniformLocation(program, "displayGPUversion"),
              displayGPUversion); // shader generation off
  printf("Changed flag to %d\n", displayGPUversion);
  glutPostRedisplay();
}

void display(void) {
  printError("pre display");

  // clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBindVertexArray(vertexArrayObjID); // Select VAO
  glDrawArrays(GL_TRIANGLES, 0, 6);    // draw object

  printError("display");

  glutSwapBuffers();
}

int main(int argc, char *argv[]) {
  glutInit(&argc, argv);
  glutInitContextVersion(3, 2);
  glutInitWindowSize(kTextureSize, kTextureSize);
  glutCreateWindow("Lab 1");
  glutDisplayFunc(display);
  glutKeyboardFunc(key);
  init();
  glutMainLoop();
}
