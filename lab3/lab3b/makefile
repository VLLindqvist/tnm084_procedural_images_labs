# set this variable to the directory in which you saved the common files
commondir = ../../common/

all : cCode executable

cCode : $(commondir)GL_utilities.c $(commondir)VectorUtils3.c $(commondir)LittleOBJLoader.c $(commondir)LoadTGA.c $(commondir)Mac/MicroGlut.m noise/noise1234.c noise/simplexnoise1234.c noise/cellular.c
	mkdir -p obj ; gcc -c -Wall -I$(commondir) -I$(commondir)Mac -I noise -DGL_GLEXT_PROTOTYPES noise/noise1234.c noise/simplexnoise1234.c noise/cellular.c $(commondir)GL_utilities.c $(commondir)LittleOBJLoader.c $(commondir)VectorUtils3.c $(commondir)LoadTGA.c $(commondir)Mac/MicroGlut.m -lm -Wno-deprecated-declarations ; mv *.o obj/

executable : lab3b.cc
	g++ -std=c++17 -Wall -o executable -I$(commondir) -I$(commondir)Mac -I noise -DGL_GLEXT_PROTOTYPES lab3b.cc obj/*.o -framework OpenGL -framework Cocoa -Wno-deprecated-declarations

clean :
	rm -rf obj ; rm -rf executable