#ifndef _GLUGG_
#define _GLUGG_

#ifdef __cplusplus
extern "C" {
#endif

// GLUGG interface
// GLUGG is the OpenGL Utilties for Geometry Generation
// By Ingemar Ragnemalm 2013-2021

#include "VectorUtils3.h"


void gluggBegin(int theMode);
void gluggVertex(float x, float y, float z);
void gluggVertexv(Point3D p);
void gluggNormal(float x, float y, float z);
void gluggNormalv(Point3D n);
void gluggTexCoord(float s, float t);
GLuint gluggEnd(int *count, GLuint program, char optimize);
void gluggDispose();

void gluggMode(int newMode);
enum
{
	GLUGG_TRIANGLES = 0,
	GLUGG_QUADS,
	GLUGG_TRIANGLE_FAN,
	GLUGG_TRIANGLE_STRIP
};

void gluggBuildBezier(Point3D *vertices, int *indices, float step);
GLint gluggBuildBezierPatchModel(Point3D *vertices, int *indices, int startPatch, int endPatch, int *count, GLuint program, float step);

// Inspect internal variables (use before gluggEnd, which clears them!)
GLfloat *gluggGetVertexArray();
GLfloat *gluggGetNormalsArray();
GLfloat *gluggGetTexCoordArray();
GLuint *gluggGetIndexArray();
int gluggGetNumVertices();
int gluggGetNumIndices();
void gluggArraysToElements(); // Needed before gluggGetNumIndices

void gluggSetPositionName(char *name);
void gluggSetNormalName(char *name);
void gluggSetTexCoordName(char *name);

// Matrix stack support
void gluggPushMatrix();
void gluggPullMatrix();
void gluggPopMatrix();
void gluggTranslate(GLfloat x, GLfloat y, GLfloat z);
void gluggRotate(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
void gluggScale(GLfloat x, GLfloat y, GLfloat z);
mat4 gluggCurrentMatrix();
void gluggSetMatrix(mat4 m);
void gluggMultMatrix(mat4 m);


#ifdef __cplusplus
}
#endif

#endif
