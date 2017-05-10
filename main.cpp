/*
 *  CSCI 444, Advanced Computer Graphics, Spring 2017
 *
 *  Project: OpenGL 4.1 Sample
 *  File: main.cpp
 *
 *  Description:
 *      This program creates a OpenGL 4.1 context using GLUT and displays a window
 *
 *  Author:
 *      Dr. Jeffrey Paone, Colorado School of Mines
 *  
 *  Notes:
 *
 */

//*************************************************************************************

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glu.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <vector>


#include "./include/OpenGLUtils.h"
#include "./include/ShaderUtils.h"
#include "./include/ShaderProgram.h"

using namespace std;

//*************************************************************************************
//
// Global Parameters

int windowWidth = 750;
int windowHeight = 750;			//size of our window

GLint leftMouseButton; 
GLint rightMouseButton;    //status of the mouse buttons
int mouseX = 0, mouseY = 0;                 //last known X and Y of the mouse

float modelPhi = 0.0f, modelTheta = 0.0f;   //use two angles to control the rotation 
                                           //of the model; use changes with mouse.

glm::vec3 cameraTPR( 1.82f, 2.01f, 45.0f );				
glm::vec3 eyePoint(   10.0f, 10.0f, 10.0f );
glm::vec3 lookAtPoint( 0.0f,  0.0f,  0.0f );
glm::vec3 upVector(    0.0f,  1.0f,  0.0f );

struct Vertex {
	float px, py, pz;
	float s, t;
};

//vertex information for the water plane, gets tesselated in a later function
float t = 50.0;
Vertex groundVertices[] = {
		{ -t, -25.0f, -t, 0.0f, 0.0f}, // 0 - BL
		{  t, -25.0f, -t, 1.0f, 0.0f}, // 1 - BR
		{  t, -25.0f,  t, 1.0f, 1.0f}, // 2 - TR
		{ -t, -25.0f,  t, 0.0f, 1.0f}  // 3 - TL
};

unsigned short groundIndices[] = {
	0, 2, 1, 	0, 3, 2
};

//vertex information for the skybox
float s = 50.0;
Vertex skybox[] = {
		//bottom
		{ -s, -s, -s, 0.0f, 0.0f},
		{  s, -s, -s, 1.0f, 0.0f},
		{  s, -s,  s, 1.0f, 1.0f},
		{ -s, -s,  s, 0.0f, 1.0f},

		//top
		{ -s, s, -s, 0.0f, 0.0f},
		{  s, s, -s, 1.0f, 0.0f},
		{  s, s,  s, 1.0f, 1.0f},
		{ -s, s,  s, 0.0f, 1.0f},
		
		//left
		{ -s, -s, -s, 1.0f, 1.0f},
		{  s, -s, -s, 0.0f, 1.0f},
		{  s, s,  -s, 0.0f, 0.0f},
		{ -s, s,  -s, 1.0f, 0.0f},
		
		//right
		{ -s, -s, s, 0.0f, 1.0f},
		{  s, -s, s, 1.0f, 1.0f},
		{  s, s,  s, 1.0f, 0.0f},
		{ -s, s,  s, 0.0f, 0.0f},
		
		//front
		{ -s, -s, -s, 0.0f, 1.0f},
		{ -s, s, -s, 0.0f, 0.0f},
		{ -s, s,  s, 1.0f, 0.0f},
		{ -s, -s,  s, 1.0f, 1.0f},
		
		//front
		{ s, -s, -s, 1.0f, 1.0f},
		{ s, s, -s, 1.0f, 0.0f},
		{ s, s,  s, 0.0f, 0.0f},
		{ s, -s,  s, 0.0f, 1.0f}
	
};

unsigned short skyboxInd[] = {
	0, 2, 1, 	0, 3, 2,
	
	0+4, 2+4, 1+4, 	0+4, 3+4, 2+4,
	
	0+8, 2+8, 1+8, 	0+8, 3+8, 2+8,
	
	0+12, 2+12, 1+12, 	0+12, 3+12, 2+12,
	
	0+16, 2+16, 1+16, 	0+16, 3+16, 2+16,
	
	0+20, 2+20, 1+20, 	0+20, 3+20, 2+20
};

GLuint vaods[2];
CSCI441::ShaderProgram* water = NULL;
CSCI441::ShaderProgram* sky = NULL;

GLuint computeShader;
GLuint computeProgram;

GLuint heightMap;

const int imageSize = 200;

float timerTime = 0;

void convertSphericalToCartesian();

Vertex verts[(imageSize+1) * (imageSize+1)];
vector<unsigned short> index_vector;
unsigned short indices[(imageSize) * (imageSize)*3*2];

GLuint skyHandles[6];

int waveControl = 0;

float wave_random;

int toggleR = 0;

//turns 1 big quad into smaller quads, used for each space on the wave
void tesselate() {
	Vertex bl = groundVertices[0];
	
	//setting new vertex
	int width = -2 * groundVertices[0].px;
	float sub = width / (float)imageSize;
	for (int i = 0; i < imageSize+1; i++) {
		for (int j = 0; j < imageSize+1; j++) {
			verts[i*(imageSize+1) + j].px = bl.px + (float)i*sub;
			verts[i*(imageSize+1) + j].py = bl.py;
			verts[i*(imageSize+1) + j].pz = bl.pz + (float)j*sub;
			verts[i*(imageSize+1) + j].s = bl.s + (float)i/imageSize;
			verts[i*(imageSize+1) + j].t = bl.t + (float)j/imageSize;
		}
	}
	
	//setting indices
	for (int i = 0; i < imageSize ; i++) {
		for (int j = 0; j < imageSize; j++) {
			
			
			index_vector.push_back((i+1)*(imageSize+1) + j+ 1);	
			index_vector.push_back(i*(imageSize+1) + j + 1);
			index_vector.push_back(i*(imageSize+1) + j);
			
			
			index_vector.push_back((i+1)*(imageSize+1) + j + 1 );
			index_vector.push_back(i*(imageSize+1) + j);
			index_vector.push_back((i+1)*(imageSize+1) + j );
			
		}
	}

	for (unsigned int i = 0; i < index_vector.size(); i++) {
		indices[i] = index_vector[i];
	}
}

//callback for resizing window
void resizeWindow(int w, int h) {
    windowWidth = w;
    windowHeight = h;

    //update the viewport to fill the window
    glViewport(0, 0, w, h);
}

//mouse click callback
void mouseCallback( int button, int state, int thisX, int thisY ) {
    //update the left and right mouse button states, if applicable
    if(button == GLUT_LEFT_BUTTON)
        leftMouseButton = state;
    else if(button == GLUT_RIGHT_BUTTON)
        rightMouseButton = state;
    
    //and update the last seen X and Y coordinates of the mouse
    mouseX = thisX;
    mouseY = thisY;
}

//mouse motion callback
void mouseMotion( int x, int y ) {
    //if the user is holding down the left mouse button, update the angles.
    if(leftMouseButton == GLUT_DOWN)
    {
        cameraTPR.x += (x - mouseX)*0.005;
        cameraTPR.y += (y - mouseY)*0.005;

        // make sure that phi stays within the range (0, M_PI)
        if(cameraTPR.y <= 0)
            cameraTPR.y = 0+0.001;
        if(cameraTPR.y >= M_PI)
            cameraTPR.y = M_PI-0.001;
        

        convertSphericalToCartesian();
    } else if(rightMouseButton == GLUT_DOWN) {
        //for the right mouse button, just determine how much the mouse has moved total.
        //not the best "zoom" behavior -- if X change is positive and Y change is negative,
        //(along the X = Y line), nothing will happen!! but it's ok if you zoom by
        //moving left<-->right or up<-->down, which works for most people i think.
        double totalChangeSq = (x - mouseX) + (y - mouseY);
        cameraTPR.z += totalChangeSq*0.01;
        

        //limit the camera radius to some reasonable values so the user can't get lost
        if(cameraTPR.z < 1.0) 
            cameraTPR.z = 1.0;
        if(cameraTPR.z > 50.0) 
            cameraTPR.z = 50.0;
		
		convertSphericalToCartesian();
    }

    mouseX = x;
    mouseY = y;
}

//key press callback
void normalKeys( unsigned char key, int x, int y ) {
    //kindly quit if the user requests!
    if (key == 'q') exit(0);
	if (key == '1') {	//different types of waves
		waveControl++;
		if (waveControl > 5) waveControl = 0;
	}
	if (key == '2') {	//toggles 'noise'
		if (toggleR == 0) toggleR = 1;
		else toggleR = 0;
	}
	if (key == '3') {	//resets random value, sometimes waves are boring with small value
		srand (time(NULL));
		int rand1 = (rand() % 10) + 1;
		int rand2 = (rand() % 100) + 10;
		wave_random = (float)rand1 / rand2;
	}
}

//time call back, used for sine waves
void myTimer(int value) {
	glutPostRedisplay();
	
	timerTime += 0.1;

    glutTimerFunc((unsigned int)(1000.0 / 60.0), myTimer, 0);
}

//renderscene, draws water and skybox
void renderScene(void) {
	
	
	
	//dispatch the compute shader
	glUseProgram(computeProgram);
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, heightMap);
	glUniform1i(glGetUniformLocation(computeProgram,"img_output"),0);
	glUniform1i(glGetUniformLocation(computeProgram,"imageSize"),imageSize);
	glUniform1f(glGetUniformLocation(computeProgram,"time"),timerTime);
	glUniform1i(glGetUniformLocation(computeProgram,"control"),waveControl);
	glUniform1f(glGetUniformLocation(computeProgram,"random"),wave_random);
	glUniform1i(glGetUniformLocation(computeProgram,"toggleR"),toggleR);
	glDispatchCompute((GLuint)imageSize,(GLuint)imageSize,1);
	
	
	//make sure writing is done before moving on
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//matrices for vertex conversion
	glm::mat4 p = glm::perspective( 45.0f, (float)(windowWidth/windowHeight), 0.1f, 200.0f );
    glm::mat4 v = glm::lookAt( eyePoint, lookAtPoint, upVector );
    glm::mat4 m;
	
	glm::mat4 vp;
	vp = glm::mat4(
								windowWidth / 2, 0.0 , 0.0, 0.0,
								0.0, windowHeight / 2, 0.0, 0.0,
								0.0, 0.0, 1.0, 0.0,
								windowWidth / 2, windowHeight / 2, 0.0, 1.0);
								
	glm::mat4 transpose = inverseTranspose(v*m);
	
	
	//call water shader
	water->useProgram();
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, heightMap);
	glUniform1i(water->getUniformLocation("image"),0);
	glUniformMatrix4fv(water->getUniformLocation("model"),1,GL_FALSE, &m[0][0]);
	glUniformMatrix4fv(water->getUniformLocation("view"),1,GL_FALSE, &v[0][0]);
	glUniformMatrix4fv(water->getUniformLocation("projection"),1,GL_FALSE, &p[0][0]);
	glUniformMatrix4fv(water->getUniformLocation("normalMat"),1,GL_FALSE, &transpose[0][0]);
	
	//draw water 
	glBindVertexArray( vaods[0] );
	glDrawElements( GL_TRIANGLES, sizeof(indices)/sizeof(unsigned short), GL_UNSIGNED_SHORT, (void*)0 );
	
	//skybox shader program
	sky->useProgram();
	glUniformMatrix4fv(sky->getUniformLocation("model"),1,GL_FALSE, &m[0][0]);
	glUniformMatrix4fv(sky->getUniformLocation("view"),1,GL_FALSE, &v[0][0]);
	glUniformMatrix4fv(sky->getUniformLocation("projection"),1,GL_FALSE, &p[0][0]);

	{
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, skyHandles[0]);
	glUniform1i(sky->getUniformLocation("image"),0);
	glBindVertexArray( vaods[1] );
	glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0 );
	
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, skyHandles[1]);
	glUniform1i(sky->getUniformLocation("image"),0);
	glBindVertexArray( vaods[1] );
	glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)(6*sizeof(unsigned short)) );
	
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, skyHandles[2]);
	glUniform1i(sky->getUniformLocation("image"),0);
	glBindVertexArray( vaods[1] );
	glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)(12*sizeof(unsigned short)) );
	
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, skyHandles[3]);
	glUniform1i(sky->getUniformLocation("image"),0);
	glBindVertexArray( vaods[1] );
	glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)(18*sizeof(unsigned short)) );
	
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, skyHandles[5]);
	glUniform1i(sky->getUniformLocation("image"),0);
	glBindVertexArray( vaods[1] );
	glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)(24*sizeof(unsigned short)) );
	
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, skyHandles[4]);
	glUniform1i(sky->getUniformLocation("image"),0);
	glBindVertexArray( vaods[1] );
	glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)(30*sizeof(unsigned short)) );
	}
	
    //push the back buffer to the screen
    glutSwapBuffers();
}

//error checking
void checkOpenGLError( std::string msg = "[WARN]" ) {
	GLenum error = glGetError();
	if( error != GL_NO_ERROR ) {
		fprintf(stderr, "%s: OpenGL Error %d: %s\n", msg.c_str(), error, gluErrorString(error));
	}
}

//converting camera location to spherical
void convertSphericalToCartesian() {
	eyePoint.x = cameraTPR.z * sinf( cameraTPR.x ) * sinf( cameraTPR.y );
	eyePoint.y = cameraTPR.z * -cosf( cameraTPR.y );
	eyePoint.z = cameraTPR.z * -cosf( cameraTPR.x ) * sinf( cameraTPR.y );
}

//basic glut calls
void setupGLUT( int argc, char *argv[] ) {
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitContextVersion(4,3);
	glutInitContextFlags(GLUT_CORE_PROFILE | GLUT_DEBUG);
    glutInitWindowPosition(50,50);
    glutInitWindowSize(windowWidth,windowHeight);
    glutCreateWindow("Final Project");

    glutKeyboardFunc(normalKeys);
    glutDisplayFunc(renderScene);
    glutReshapeFunc(resizeWindow);
    glutMouseFunc(mouseCallback);
    glutMotionFunc(mouseMotion);
	glutTimerFunc((unsigned int)(1000.0 / 60.0), myTimer, 0);
}

//basic opengl calls
bool setupOpenGL() {
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LESS );
	glCullFace( GL_CW );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

	glewExperimental = GL_TRUE;
	GLenum glewResult = glewInit();

	/* check for an error */
	if( glewResult != GLEW_OK ) {
		printf( "[ERROR]: Error initalizing GLEW\n");
		exit(EXIT_FAILURE);
	} else if( GLEW_VERSION_4_3 ) {
		printf("[INFO]: HOORAY!  Driver supports OpenGL 4.3\n");
	} else {
		printf("[ERROR]: Driver does not support OpenGL 4.3\n\nShutting Down...\n");
		return false;
	}
	
	CSCI441::OpenGLUtils::printOpenGLInfo();
	
	int work_grp_cnt[3];

	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);

	printf("max global (total) work group size x:%i y:%i z:%i\n", work_grp_cnt[0], work_grp_cnt[1], work_grp_cnt[2]);
	
	int work_grp_size[3];

	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);

	printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n", work_grp_size[0], work_grp_size[1], work_grp_size[2]);
	
	int work_grp_inv;
	
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
	printf("max local work group invocations %i\n", work_grp_inv);
	
	return true;
}

//reading from file, used for setting up compute shader since provided functions do not use compute shader
void readTextFromFile(string filename, char* &output){
    string buf = string("");
    string line;
    
    ifstream in(filename.c_str());
    while( getline(in, line) ) {
        buf += line + "\n";
    }
    output = new char[buf.length()+1];
    strncpy(output, buf.c_str(), buf.length());
    output[buf.length()] = '\0';
    
    in.close();
}

//sets up all shaders, does extra work for compute shader
void setupShaders() {
	water = new CSCI441::ShaderProgram("shaders/water.v.glsl","shaders/water.g.glsl","shaders/water.f.glsl");
	sky = new CSCI441::ShaderProgram("shaders/sky.v.glsl","shaders/sky.f.glsl");
	
	//compute shader code, 
	computeShader = glCreateShader(GL_COMPUTE_SHADER);
	char *shader_string;
	string src = "shaders/compute.c.glsl";
	readTextFromFile(src,shader_string);
	glShaderSource(computeShader, 1, (const char**)&shader_string, NULL);
	glCompileShader(computeShader);
	
	CSCI441::ShaderUtils::printLog(computeShader);
	
	computeProgram = glCreateProgram();
	glAttachShader(computeProgram,computeShader);
	glLinkProgram(computeProgram);
	
	CSCI441::ShaderUtils::printLog(computeProgram);
}

//sets up buffers, textures etc
void setupBuffers() {
	
	//skybox textures
	skyHandles[0] = CSCI441::OpenGLUtils::registerOpenGLTexture2D("textures/miramar_dn.tga",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_RGB);
	if (skyHandles[0] == 0) printf("error reading texture.\n");
	skyHandles[1] = CSCI441::OpenGLUtils::registerOpenGLTexture2D("textures/miramar_up.tga",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_RGB);
	skyHandles[2] = CSCI441::OpenGLUtils::registerOpenGLTexture2D("textures/miramar_lf.tga",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_RGB);
	skyHandles[3] = CSCI441::OpenGLUtils::registerOpenGLTexture2D("textures/miramar_rt.tga",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_RGB);
	skyHandles[4] = CSCI441::OpenGLUtils::registerOpenGLTexture2D("textures/miramar_ft.tga",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_RGB);
	skyHandles[5] = CSCI441::OpenGLUtils::registerOpenGLTexture2D("textures/miramar_bk.tga",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_RGB);
	
	//heightmap buffer
	glGenTextures(1,&heightMap);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightMap);  
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, imageSize, imageSize, 0, GL_RGBA, GL_FLOAT,NULL);
	glBindImageTexture(0, heightMap, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	
	//vertex array objects
	glGenVertexArrays( 2, vaods );
	GLuint vbods[2];

	//ground bindings
	glBindVertexArray( vaods[0] );
	glGenBuffers( 2, vbods );
	glBindBuffer( GL_ARRAY_BUFFER, vbods[0] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vbods[1] );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW );
	glEnableVertexAttribArray( water->getAttributeLocation("vertexPosition") );
	glVertexAttribPointer( water->getAttributeLocation("vertexPosition"), 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*) 0 );
	glEnableVertexAttribArray( water->getAttributeLocation("texCoords") );
	glVertexAttribPointer( water->getAttributeLocation("texCoords"), 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*) (3*sizeof(float)) );
	
	
	//skybox bindings
	glBindVertexArray( vaods[1] );
	glGenBuffers( 2, vbods );
	glBindBuffer( GL_ARRAY_BUFFER, vbods[0] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(skybox), skybox, GL_STATIC_DRAW );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vbods[1] );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxInd), skyboxInd, GL_STATIC_DRAW );
	glEnableVertexAttribArray( sky->getAttributeLocation("vertexPosition") );
	glVertexAttribPointer( sky->getAttributeLocation("vertexPosition"), 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*) 0 );
	glEnableVertexAttribArray( sky->getAttributeLocation("texCoords") );
	glVertexAttribPointer( sky->getAttributeLocation("texCoords"), 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*) (3*sizeof(float)) );
	
}

//main
int main( int argc, char *argv[] ) {
	setupGLUT( argc, argv );
	if( !setupOpenGL() ) {
		return EXIT_FAILURE;
	}
	
	//set random value once
	srand (time(NULL));
	int rand1 = (rand() % 10) + 1;
	int rand2 = (rand() % 100) + 10;
	wave_random = (float)rand1 / rand2;
	
	//generate new quads
	tesselate();
	
	//set up all necessary functionality
	setupShaders();
	setupBuffers();
	
	

	//and enter the GLUT loop, never to exit.
    glutMainLoop();
	
	delete water;
	delete sky;
	
	return EXIT_SUCCESS;
}
