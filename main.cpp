#include <stdio.h>     // - Just for some ASCII messages
#include <stdlib.h>
#include <time.h>
#include <GL/freeglut.h>
#include "visuals.h"   // Header file for our OpenGL functions

/////////////// Main Program ///////////////////////////

int main(int argc, char* argv[])
{ 
  srand(time(NULL));
  init_stars();
  // initialize GLUT library state
  glutInit(&argc, argv);

  glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH|GLUT_DOUBLE);
 
  glutInitWindowSize(1000,500);
  glutInitWindowPosition(50,50);
  
  glutCreateWindow("Planet");
 
  Setup();
 
  glutDisplayFunc(Render);
  glutReshapeFunc(Resize);
  glutIdleFunc(Idle);
  glutKeyboardFunc(Keyboard);
  glutMouseFunc(Mouse);

  glutCreateMenu(MenuSelect);
  glutAddMenuEntry("Red",RED);
  glutAddMenuEntry("Blue",BLUE);
  glutAddMenuEntry("Green",GREEN);
  glutAddMenuEntry("White",WHITE);
  
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  glutMainLoop();
  return 0;	
}  
