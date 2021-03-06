#include <stdlib.h>
#include <time.h>
#include <stdio.h>     // - Just for some ASCII messages
#include <string.h>
#include <math.h>
#include <fstream>
#include <iostream>
#include <GL/freeglut.h>

#include "visuals.h"   // Header file for our OpenGL functions

// this is for the mallocs to read from file
// we could use 18200,7200 but this works fine
#define SIZE_POINT 9999
#define SIZE_FACE 19000

model md;
int i = 0;
static float tx = 0.0;
static float radius = 0;
static float transparency = 0;
static float t = 0;

/*== rotation*/
static float rotx = 0.0;
static float roty = 0.0;
static float rotz = 0.0;

static float planetrot = 0.0;
static bool animate = true;
static float red = 1.0;
static float green = 0.0;
static float blue = 0.0;
static float x[STARS_FRONT], y[STARS_FRONT], z[STARS_FRONT];

//RAND BOUNDS
#define MIN 1
#define MAX 300

using namespace std;

//This example shows how to create an array of random floating-point numbers that are drawn from a uniform distribution in the open interval 
static inline float rand_back (int a, int b) {
	return ((b-a)* (float(rand()) / float(RAND_MAX)) + a);
}

// Stars also front
void init_stars() {
	for (int i = 0; i < STARS_FRONT; i++) {
		x[i] = rand_back(MIN,MAX);
		if (rand()%2 != 0) x[i] *= -1;

		y[i] = rand_back(MIN,MAX);
		if (rand()%2 != 0) y[i] *= -1;
		
		z[i] = rand_back(MIN,MAX);
		if (rand()%2 != 0) z[i] *= -1;
	}
}


void DisplayStars() {
	for (int i = 0; i < STARS_FRONT; i++) {
		glPushMatrix();
		glTranslatef(x[i], y[i], z[i]);

		glPushMatrix();
		glColor3f(0.6, 0.6, 0.6);
		glutSolidSphere(0.8, 8, 8);
		glPopMatrix();
		glPopMatrix();
	}
}

void Render()
{    
	//CLEARS FRAME BUFFER ie COLOR BUFFER& DEPTH BUFFER (1.0)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clean up the colour of the window
													   // and the depth buffer
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();
	
	DisplayAxes();
	
	glTranslatef(0.0, 0.0, -300);
	glRotatef(rotx, 1, 0, 0);
	glRotatef(roty, 0, 1, 0);
	glRotatef(rotz, 0, 0, 1);

	DisplayStars();
	DisplayPlanetAndMoon();
	DisplaySun();

	glutSwapBuffers();             // All drawing commands applied to the 
								 // hidden buffer, so now, bring forward;
								 // the hidden buffer and hide the visible one
}

//-----------------------------------------------------------
void Resize(int w, int h)
{ 
	// define the visible area of the window ( in pixels )
	if (h==0) h=1;
	glViewport(0,0,w,h); 

	// Setup viewing volume
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity();

	gluPerspective(60.0, (float)w/(float)h, 1.0, 700.0);
}

static float t2 = 0;
void Idle()
{
	/*== animation of the sun*/
	if (animate)
	{
		radius = 800 + sin(t) * 300;
		transparency = sin(t);
		planetrot += 0.5f;
	}
	t += 0.02;
	
	glutPostRedisplay();
}

void Keyboard(unsigned char key,int x,int y)
{
	switch(key)
	{
		case 'q': exit(0);
			break;
		case 'p': animate = !animate;
			break;
		case 'a': roty -=5.0f;
			break;
		case 'd': roty +=5.0f;
			break;
		case 'w': rotx -= 5.0f;
			break;
		case 's': rotx += 5.0f;
			break;
		case 'z': rotz += 5.0f;
			break;
		case 'c': rotz -= 5.0f;
			break;
		default : break;
	}

	glutPostRedisplay();

}

void Mouse(int button,int state,int x,int y)
{
	 if(state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
	 {
		 animate = !animate;
         glutPostRedisplay();
	 }
}


void Setup()  // TOUCH IT !! 
{ 
	ReadFile(&md);

	//Parameter handling
	glShadeModel (GL_SMOOTH);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);  //renders a fragment if its z value is less or equal of the stored value
	glClearDepth(1);
    
	// polygon rendering mode
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );
  	glEnable(GL_NORMALIZE);

	//Set up light source
	//GLfloat light_position[] = { 0.0, 30.0, 300.0, 0.0 };
	GLfloat light_position[] = { 0.0, 30.0, 50.0, 0.0 };
	//GLfloat light_position[] = { 0.0, 0.0, 100.0, 0.0 };
	
	glLightfv( GL_LIGHT0, GL_POSITION, light_position);

	GLfloat ambientLight[] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat diffuseLight[] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat specularLight[] = { 1.0, 1.0, 1.0, 1.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuseLight );
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);

	glFrontFace(GL_CCW);

	/*== enable GL_BLEND for transparency*/
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Black background
	glClearColor(0.0f,0.0f,0.0f,1.0f);
}

void ReadFile(model *md)
{
	char temp_w[128];
	int v = 0;
	int vn = 0;
	int f = 0;

	FILE* obj_file = fopen("planet.obj", "r");
	if (obj_file == NULL) {
		std::cout << "Wrong file,try again\n";
		return;
	}

	md->faces = 0;
	md->obj_points = (point*)malloc(sizeof(point) * SIZE_POINT);
	md->obj_faces = (face*)malloc(sizeof(face) * SIZE_FACE);

	if (md->obj_points == NULL || md->obj_faces == NULL) {
		std::cout << "Malloc error\n";
		return;
	}

	while (fscanf(obj_file, "%s", temp_w) != EOF) {
		if (!strcmp(temp_w, "v")) {
			fscanf(obj_file, "%f %f %f\n", &(md->obj_points[v].x), &(md->obj_points[v].y), &(md->obj_points[v].z));
			v++;
		}
		else if (!strcmp(temp_w, "f")) {
			fscanf(obj_file, "%d//%d %d//%d %d//%d\n", &(md->obj_faces[f].vtx[0]), &(md->obj_faces[f].vtxn[0]), &(md->obj_faces[f].vtx[1]), &(md->obj_faces[f].vtxn[1]), &(md->obj_faces[f].vtx[2]), &(md->obj_faces[f].vtxn[2]));
			md->faces++;
			f++;
		}
	}
	md->vertices = v;
	fclose(obj_file);
}
 
void DisplaySun()
{
	/*== sun*/
	glPushMatrix();
		glScalef(0.06, 0.06, 0.06);
		glColor3f(255, 255, 0);
		glutSolidSphere(500, 100, 100);
	glPopMatrix();

	/*== radiation*/
	glPushMatrix();
		glScalef(0.05, 0.05, 0.05);
		glColor4f(253, 255, 0, transparency);
		glutSolidSphere(radius, 20, 20);
	glPopMatrix();
}

void DisplayPlanetAndMoon()
{
	/*== planet: rotate around the sun*/
	glPushMatrix();
	glRotatef(0.5*planetrot, 0.0, 1.0, 0.0);
	glTranslatef(150, 0, 0);

	/*== planet: rotate around itself*/
	glPushMatrix();

	// uncomment if we want earth to rotate from her self
	//glRotatef(2*planetrot, 0.0, 1.0, 0.0);

	glColor3f(0.0, 0.0, 1.0);
	glScalef(0.05, 0.05, 0.05);
	DisplayModel(md);
	glPopMatrix();

	/*== moon: rotate around planet*/
	glRotatef(5*planetrot, 1.0, 0.0, 0.0);
	glTranslatef(0, 50, 0);
	glColor3f( 0.4, 0.4, 0.4);
	glScalef(0.01, 0.01, 0.01);

	/*== moon: rotate around itself -- moon rotates around itself every 27 days*/
	glRotatef(0.1*planetrot, 0.0, 1.0, 0.0);
	DisplayModel(md);
	glPopMatrix();
}

void DisplayAxes()
{
	glPushMatrix();
		glColor3f(255, 255, 255);
		glTranslatef(0, 0, -150);
		glRotatef(rotx, 1.0, 0.0, 0.0);
  		glRotatef(roty, 0.0, 1.0, 0.0);
  		glRotatef(rotz, 0.0, 0.0, 1.0);
		
		glBegin(GL_LINES);

		/*== draw line for x axis*/
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(50.0, 0.0, 0.0);

		/*== draw line for y axis*/
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 50.0, 0.0);

		glEnd();
	glPopMatrix();
}

void DisplayModel(model md)
{
	glPushMatrix();
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < md.faces; i++)
	{
		glNormal3f(md.obj_points[md.obj_faces[i].vtxn[0] - 1].x, md.obj_points[md.obj_faces[i].vtxn[0] - 1].y, md.obj_points[md.obj_faces[i].vtxn[0] - 1].z);
		glVertex3f(md.obj_points[md.obj_faces[i].vtx[0]-1].x,md.obj_points[md.obj_faces[i].vtx[0]-1].y,md.obj_points[md.obj_faces[i].vtx[0]-1].z);
		glNormal3f(md.obj_points[md.obj_faces[i].vtxn[1] - 1].x, md.obj_points[md.obj_faces[i].vtxn[1] - 1].y, md.obj_points[md.obj_faces[i].vtxn[1] - 1].z);
		glVertex3f(md.obj_points[md.obj_faces[i].vtx[1]-1].x,md.obj_points[md.obj_faces[i].vtx[1]-1].y,md.obj_points[md.obj_faces[i].vtx[1]-1].z);
		glNormal3f(md.obj_points[md.obj_faces[i].vtxn[2] - 1].x, md.obj_points[md.obj_faces[i].vtxn[2] - 1].y, md.obj_points[md.obj_faces[i].vtxn[2] - 1].z);
		glVertex3f(md.obj_points[md.obj_faces[i].vtx[2]-1].x,md.obj_points[md.obj_faces[i].vtx[2]-1].y,md.obj_points[md.obj_faces[i].vtx[2]-1].z);
	}
	glEnd();
	glPopMatrix();
}
