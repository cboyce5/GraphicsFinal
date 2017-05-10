########################################
## SETUP MAKEFILE
##      Set the appropriate TARGET (our
## executable name) and any OBJECT files
## we need to compile for this program.
##
## Next set the path to our local
## include/, lib/, and bin/ folders.
## (If you we are compiling in the lab,
## then you can ignore these values.
## They are only for if you are
## compiling on your personal machine.)
##
## Set if we are compiling in the lab
## environment or not.  Set to:
##    1 - if compiling in the Lab
##    0 - if compiling at home
##
## Finally, set the flags for which
## libraries are using and want to
## compile against.
########################################

TARGET = finalProject
OBJECTS = main.o src/OpenGLUtils.o src/ShaderProgram.o src/ShaderUtils.o

CXX    = C:/Rtools/mingw_64\bin\g++.exe
CFLAGS = -Wall -g

INCPATH += -I./include
LIBPATH += -L./lib

#############################
## SETUP OpenGL & GLUT 
#############################

INCPATH += -IC:/opengl/include
LIBPATH += -LC:/opengl/lib
LIBS +=  -lopengl32 -lglut -lglu32

#############################
## SETUP GLEW 
#############################

INCPATH += -I./include
LIBPATH += -L./lib
LIBS += -lglew32

#############################
## COMPILATION INSTRUCTIONS 
#############################

all: $(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)

.c.o: 	
	$(CXX) $(CFLAGS) $(INCPATH) -c -o $@ $<

.cc.o: 	
	$(CXX) $(CFLAGS) $(INCPATH) -c -o $@ $<

.cpp.o: 	
	$(CXX) $(CFLAGS) $(INCPATH) -c -o $@ $<

$(TARGET): $(OBJECTS) 
	$(CXX) $(CFLAGS) $(INCPATH) -o $@ $^ $(LIBPATH) $(LIBS)
