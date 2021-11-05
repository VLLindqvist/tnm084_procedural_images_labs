# set this variable to the directory in which you saved the common files
commondir = ../common/

all : lab1-1

lab1-1 : lab1.c $(commondir)GL_utilities.c $(commondir)VectorUtils3.c $(commondir)LittleOBJLoader.c $(commondir)LoadTGA.c $(commondir)Mac/MicroGlut.m noise/noise1234.c noise/simplexnoise1234.c noise/cellular.c
	gcc -Wall -o lab1-1 -I$(commondir) -I../common/Mac -I noise -DGL_GLEXT_PROTOTYPES lab1.c  noise/noise1234.c noise/simplexnoise1234.c noise/cellular.c $(commondir)GL_utilities.c $(commondir)LittleOBJLoader.c $(commondir)VectorUtils3.c $(commondir)LoadTGA.c $(commondir)Mac/MicroGlut.m -framework OpenGL -framework Cocoa -lm -Wno-deprecated-declarations

clean :
	rm lab1-1
