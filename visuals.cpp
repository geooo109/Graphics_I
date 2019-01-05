#include <stdlib.h>
#include <time.h>
#include <stdio.h>     // - Just for some ASCII messages
#include <string.h>
#include <math.h>
#include <fstream>
#include <GL/freeglut.h>

#include "visuals.h"   // Header file for our OpenGL functions

model md;
int i = 0;
static float tx = 0.0;
static float radius = 0;
static float opac = 0;
static float star_radius =  0 ;
static float t2 = 0;
static float t = 0;
static float rotx = 0.0;
static float roty = 0.0;
static float planetrot = 0.0;
static bool animate = true;
static bool animate_star = true;
static float red = 1.0;
static float green = 0.0;
static float blue = 0.0;
static int x[100], y[100], z[100];

using namespace std;

void init_stars()
{
	srand(time(NULL));
	for (int i = 0; i < 100; i++)
	{
		x[i] = rand() % STARS - 900;
		y[i] = rand() % STARS - 900;
		z[i] = rand() % STARS - 900;
	}
}

void Render()
{    
	//CLEARS FRAME BUFFER ie COLOR BUFFER& DEPTH BUFFER (1.0)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clean up the colour of the window
													   // and the depth buffer
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();

	glTranslatef(0.0, 0.0, -100);
	glRotatef(rotx, 0, 1, 0);
	glRotatef(roty, 1, 0, 0);

	DisplayAxes();
	DisplaySun();
	DisplayPlanet();
	DisplayMoon();


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

void Idle()
{
	//Animation hliou
	if (animate)
		radius = 700 + sin(t) * 300;
		opac = sin(t);
	t += 0.1;
	//Animation asteriwn
	
	if (animate)
			star_radius = 0.5 + sin(t2)*0.3;
	t2 += 0.2;
	if (animate) {
		planetrot += 0.5f;
	}

	glutPostRedisplay();
}

void Keyboard(unsigned char key,int x,int y)
{
	switch(key)
	{
	case 'q' : exit(0);
		break;
	case 'a' : rotx-=0.5f;
		break;
	case 'd' : rotx+=0.5f;
		break;
	case 'w': roty -= 0.5f;
		break;
	case 's': roty += 0.5f;
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
  
	//Set up light source
	GLfloat light_position[] = { 0.0, 0.0, -100.0, 0.0 };
	
	glLightfv( GL_LIGHT0, GL_POSITION, light_position);

	GLfloat ambientLight[] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat diffuseLight[] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat specularLight[] = { 1.0, 1.0, 1.0, 1.0 };

	glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuseLight );
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);

	glFrontFace(GL_CCW);

	// Black background
	glClearColor(0.0f,0.0f,0.0f,1.0f);
}

void MenuSelect(int choice)
{
	switch (choice) 
	{
		case RED : 
			red = 1.0; 
			green = 0.0; 
			blue = 0.0; break;
		case GREEN : 
			red = 0.0; 
			green = 1.0; 
			blue = 0.0; break;
		case BLUE : 
			red = 0.0; 
			green = 0.0; 
			blue = 1.0; break;
		case WHITE : 
			red = 1.0; 
			green = 1.0; 
			blue = 1.0; break;
	}
}

void ReadFile(model *md)
{
	FILE* obj_file = fopen("planet.obj", "r");
	md->faces = 0;
	md->obj_points = (point*)malloc(sizeof(point) * 9122);
	md->obj_faces = (face*)malloc(sizeof(face) * 18240);
	char word[64];
	int v = 0;
	int vn = 0;
	int f = 0;

	while (1)
	{
		if (fscanf(obj_file, "%s", word) == EOF)
			break;
		else
		{
			if (strcmp(word, "v") == 0)
			{
				fscanf(obj_file, "%f %f %f\n", &(md->obj_points[v].x), &(md->obj_points[v].y), &(md->obj_points[v].z));
					v++;
			}
			else if (strcmp(word, "f") == 0)
			{
				md->faces++;
				fscanf(obj_file, "%d//%d %d//%d %d//%d\n", &(md->obj_faces[f].vtx[0]), &(md->obj_faces[f].vtxn[0]), &(md->obj_faces[f].vtx[1]), &(md->obj_faces[f].vtxn[1]), &(md->obj_faces[f].vtx[2]), &(md->obj_faces[f].vtxn[2]));
					f++;
			}
		}
	}
	fclose(obj_file);
	md->vertices = v;
}
 
void DisplaySun()
{
	glDisable(GL_BLEND);

	/*== sun*/
	glPushMatrix();
		glScalef(0.02, 0.02, 0.02);
		glTranslatef(0.0, 0.0, -800.0);
		glColor4f(255, 255, 0,255);
		glutSolidSphere(700,100,100);
	glPopMatrix();

	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/*== radiation*/
	glPushMatrix();
		glScalef(0.02, 0.02, 0.02);
		glTranslatef(0.0, 0.0, -800);
		glRotatef(rotx, 1, 0, 0);
		glColor4f(253,255,0,opac);
		glutSolidSphere(radius, 20, 20);
	glPopMatrix();
}

void DisplayPlanet()
{
	glPushMatrix();
		glRotatef(0.5*planetrot, 0, 1, 0); //kinhsh gurw ap ton hlio
		glScalef(0.02, 0.02, 0.02);
		glTranslatef(2000, 0.0, -1500);
		glRotatef(planetrot, 0, 1, 0);   
		glColor3f(0.0, 0.0, 1.0);   // Set drawing colour
		glScalef(0.8, 0.8, 0.8);
		DisplayModel(md);
	}

	void DisplayMoon()
	{
		glPushMatrix(); 
			glRotatef(4*planetrot, 0, 0, 1);
			glScalef(0.3,0.3,0.3);
			glTranslatef(3200, 200, -1000);
			glRotatef(5*planetrot, 1, 0, 0);
			glColor3f(0.5, 0.5, 0.5);
			DisplayModel(md);
		glPopMatrix();
	}

void DisplayAxes()
{
	glPushMatrix();
		glBegin(GL_LINES);

		/*== draw line for x axis*/
		glColor3f(1.0, 1.0, 1.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(1000.0, 0.0, 0.0);

		/*== draw line for y axis*/
		glColor3f(1.0, 1.0, 1.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 1000.0, 0.0);
		glEnd();
	glPopMatrix();
}

	void DisplayModel(model md)
	{
		glPushMatrix();
		glBegin(GL_TRIANGLES);
	glEnable(GL_NORMALIZE);
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
