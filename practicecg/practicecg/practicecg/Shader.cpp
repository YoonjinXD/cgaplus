/** Simple OpenGL Program
*=============================================================================!
*
*  Program: simple_OpenGL.cpp
*
*  by (BeH)
*
*  A simple, introductory OpenGL program.
*
*=============================================================================!
*/
#include <stdio.h>
#include <stdlib.h>

#include <gl/glew.h>
#include <gl/glut.h>



// function prototypes
void showOpenGLInfo(void);
void display(void);



/******************************************************************************!
* Main OpenGL program
******************************************************************************!
*/
int main(int argc, char *argv[])
{
	// init GLUT and create Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(320, 320);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("OpenGL: A Simple Window");
	showOpenGLInfo();



	// register callbacks
	glutDisplayFunc(display);



	// enter GLUT event processing cycle
	glutMainLoop();



	return 0;
}



/******************************************************************************!
* show OpenGL Info through the Console
******************************************************************************!
*/
void  showOpenGLInfo(void)
{
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(glewInit()));
		exit(0);
	}
	fprintf(stderr, "OpenGL Vender: %s\n", glGetString(GL_VENDOR));
	fprintf(stderr, "OpenGL Version: %s\n", glGetString(GL_VERSION));
	fprintf(stderr, "GLU Version: %s\n", gluGetString(GLU_VERSION));
	fprintf(stderr, "GLEW Version: %s\n", glewGetString(GLEW_VERSION));



	return;
}



/******************************************************************************!
* draw graphics
******************************************************************************!
*/
void  display(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);



	glColor4f(1.0, 1.0, 0.0, 1.0);
	glBegin(GL_TRIANGLES);
	glVertex3f(-0.5, -0.5, 0.0);
	glVertex3f(0.5, -0.5, 0.0);
	glVertex3f(0.0, 0.5, 0.0);
	glEnd();



	glFlush();
}

