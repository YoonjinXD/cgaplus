#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <gl/glew.h>
#include <gl/GL.H>
#include <gl/GLU.H>
#include <gl/glut.h>
#include "Matrix.h"
#include "WaveFrontOBJ.h"
#include "particle.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include<algorithm>


<<<<<<< HEAD
=======

>>>>>>> 4352909dc06d9e6cbbc55ec712f97b6f66dc47fb
// edited
struct list1 {
	float pos;
	int id;
	bool operator()(list1 p, list1 q) {
		if (p.pos - q.pos < 0)return false;
		return true;
	}
}foldCtnLoc1[15], spreadCtnLoc1[15];

struct list2 {
	float pos;
	int id;
	bool operator()(list2 p, list2 q) {
		if (p.pos - q.pos > 0)return false;
		return true;
	}
}foldCtnLoc2[15], spreadCtnLoc2[15];



#define L 100
using namespace std;

// 'cameras' stores infomation of 5 cameras.
float cameras[9] = { 28, 18, 28, 0, 0, -1, 0, 1, 0 };


Matrix wld2cam, cam2wld;
// texture mapping
GLuint photo, photo2;
Matrix ctn2wld;
WaveFrontOBJ* ctn;
mass_cloth *cloth;
Matrix ctn22wld;
WaveFrontOBJ* ctn2;
mass_cloth *cloth2;
GLuint plt1Photo, plt2Photo, plt3Photo;
Matrix plt12wld, plt22wld, plt32wld;
WaveFrontOBJ* plt1, *plt2, *plt3;
Matrix bong2wld;
WaveFrontOBJ* bong;
GLuint RoomPhoto, SkyPhoto, MoonPhoto;
Matrix room2wld, sky2wld, moon2wld;
WaveFrontOBJ* room, *sky;
WaveFrontOBJ *moon;
GLuint tvPhoto;
Matrix tv2wld;
WaveFrontOBJ* TV;

GLuint ScreenTexture;

int frame = 0;
int width, height;
int oldX, oldY;

int selectMode;
bool picking = false;

/*******************************************************************/
//(PA #2,#3) 수행한 내용 추가
/*******************************************************************/
float moveX; //x축 마우스 드래그 정도
float moveY; //y축 마우스 드래그 정도
int current_obj = 0; //현재 선택된 object를 저장. 0=cow, 1=bunny. 초기값은 cow
float camview[3] = { (cameras[3]), (cameras[4]),(cameras[5]) };

/*PA4 추가 변수*/
bool point = false; //point light on/off
bool direc = false; //direction light on/off
bool spot = false; //spot light on/off

bool shademodel = false;
bool tvon = false;

//------------------------------------------------------------------------------
void munge(int x, float& r, float& g, float& b)
{
	r = (x & 255) / float(255);
	g = ((x >> 8) & 255) / float(255);
	b = ((x >> 16) & 255) / float(255);
}



//------------------------------------------------------------------------------
int unmunge(float r, float g, float b)
{
	return (int(r) + (int(g) << 8) + (int(b) << 16));
}


// Texture Mapping
GLuint LoadTexture(char *texfile) {
	GLuint texture = NULL;
	glGenTextures(1, &texture);

	//이미지 로드하고
	FILE *fp = fopen(texfile, "rb");
	if (!fp) {
		printf("ERROR : No %s.\n fail to bind %d\n", texfile, texture);
		return false;
	}

	int width, height, channel;
	unsigned char *image = stbi_load_from_file(fp, &width, &height, &channel, 4);
	fclose(fp);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image);

	return texture;
}

/*********************************************************************************
* Draw x, y, z axis of current frame on screen.
* x, y, and z are corresponded Red, Green, and Blue, resp.
**********************************************************************************/
void drawAxis(float len)
{
	glDisable(GL_LIGHTING);		// Lighting is not needed for drawing axis.
	glBegin(GL_LINES);			// Start drawing lines.
	glColor3d(1, 0, 0);			// color of x-axis is red.
	glVertex3d(0, 0, 0);
	glVertex3d(len, 0, 0);		// Draw line(x-axis) from (0,0,0) to (len, 0, 0). 
	glColor3d(0, 1, 0);			// color of y-axis is green.
	glVertex3d(0, 0, 0);
	glVertex3d(0, len, 0);		// Draw line(y-axis) from (0,0,0) to (0, len, 0).
	glColor3d(0, 0, 1);			// color of z-axis is  blue.
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, len);		// Draw line(z-axis) from (0,0,0) - (0, 0, len).
	glEnd();					// End drawing lines.
	glEnable(GL_LIGHTING);
}

void moveCtn() {
	if (GetAsyncKeyState('O')) {
		for (int i = 0; i < cloth->fp.size(); i++)
		{
			if (cloth->fp[foldCtnLoc1[i].id]->position.x <= foldCtnLoc1[i].pos) {
				cloth->fp[foldCtnLoc1[i].id]->position.x += 1;
			}
		}
		for (int i = 0; i < cloth2->fp.size(); i++)
		{
			if (cloth2->fp[foldCtnLoc2[i].id]->position.x >= foldCtnLoc2[i].pos) {
				cloth2->fp[foldCtnLoc2[i].id]->position.x -= 1;
			}
		}
	}
	else if (GetAsyncKeyState('C')) {
		for (int i = 0; i < cloth->fp.size(); i++)
		{
			if (cloth->fp[spreadCtnLoc1[i].id]->position.x >= spreadCtnLoc1[i].pos) {
				cloth->fp[spreadCtnLoc1[i].id]->position.x -= 1;
			}
		}
		for (int i = 0; i < cloth2->fp.size(); i++)
		{
			if (cloth2->fp[spreadCtnLoc2[i].id]->position.x <= spreadCtnLoc2[i].pos) {
				cloth2->fp[spreadCtnLoc2[i].id]->position.x += 1;
			}
		}
	}
}

void TVon() {
	if (tvon) {
		glGenTextures(1, &ScreenTexture);
		glReadBuffer(GL_FRONT);
		glBindTexture(GL_TEXTURE_2D, ScreenTexture);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, width, height, 0);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		// Drawing the saved screen texture back to the screen:
		glBindTexture(GL_TEXTURE_2D, ScreenTexture);
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(-51.68, 44.126, -34.625);
		glTexCoord2f(1, 0);
		glVertex3f(-51.68, 44.126, 18.039);
		glTexCoord2f(1, 1);
		glVertex3f(-51.68, 73.052, 18.039);
		glTexCoord2f(0, 1);
		glVertex3f(-51.68, 73.052, -34.039);
		glEnd();
	}
}


void InitCamera(){

	// initialize camera frame transforms.
	float* c = cameras;													// 'c' points the coordinate of i-th camera.
	wld2cam.set(Matrix().m);											// Insert {0} matrix to wld2cam vector.
	glPushMatrix();															// Push the current matrix of GL into stack.

	glLoadIdentity();														// Set the GL matrix Identity matrix.
	gluLookAt(c[0], c[1], c[2], c[0] + camview[0], c[1] + camview[1], c[2] + camview[2], c[6], c[7], c[8]);		// Setting the coordinate of camera.
	glGetFloatv(GL_MODELVIEW_MATRIX, wld2cam.matrix());					// Read the world-to-camera matrix computed by gluLookAt.
	cam2wld.set(wld2cam.inverse().m);								// Get the camera-to-world matrix.

	glPopMatrix();															// Transfer the matrix that was pushed the stack to GL.
	
}


void moveCam() {
	float* c = cameras;
	glPushMatrix();
	glLoadIdentity();
	
	float delta = 2;

	if (GetAsyncKeyState('W'))
	{
		c[0] -= cam2wld.m[2][0] * delta;
		c[1] -= cam2wld.m[2][1] * delta;
		c[2] -= cam2wld.m[2][2] * delta;
	}
	if (GetAsyncKeyState('S'))
	{
		c[0] += cam2wld.m[2][0] * delta;
		c[1] += cam2wld.m[2][1] * delta;
		c[2] += cam2wld.m[2][2] * delta;
	}
	if (GetAsyncKeyState('A'))
	{
		c[0] -= cam2wld.m[0][0] * delta;
		c[1] -= cam2wld.m[0][1] * delta;
		c[2] -= cam2wld.m[0][2] * delta;
	}
	if (GetAsyncKeyState('D'))
	{
		c[0] += cam2wld.m[0][0] * delta;
		c[1] += cam2wld.m[0][1] * delta;
		c[2] += cam2wld.m[0][2] * delta;
	}
	if (GetAsyncKeyState('Z')) {
		c[1] -= delta;
		if (c[1] > room->bbmin.pos.y) {
			camview[1] -= delta*0.0001;
		}
	}
	if (GetAsyncKeyState('Q'))
	{
		c[1] += delta;
		if (c[1] < room->bbmax.pos.y) {
			camview[1] += delta*0.0001;
		}
	}

	if (c[0] < room->bbmin.pos.x) {
		c[0] = room->bbmin.pos.x;
	}
	if (c[0] > room->bbmax.pos.x) {
		c[0] = room->bbmax.pos.x;
	}
	if (c[1] < room->bbmin.pos.y) {
		c[1] = room->bbmin.pos.y;
	}
	if (c[1] > room->bbmax.pos.y) {
		c[1] = room->bbmax.pos.y;
	}
	if (c[2] < room->bbmin.pos.z) {
		c[2] = room->bbmin.pos.z;
	}
	if (c[2] > room->bbmax.pos.z) {
		c[2] = room->bbmax.pos.z;
	}
	gluLookAt(c[0], c[1], c[2], c[0] + camview[0], c[1] + camview[1], c[2] + camview[2], c[6], c[7], c[8]);
	glGetFloatv(GL_MODELVIEW_MATRIX, wld2cam.matrix());
	cam2wld.set(wld2cam.inverse().m);
	
	glPopMatrix();
	glutPostRedisplay();
}
void normalize(float *v)
{
	float magnitude = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] /= magnitude;
	v[1] /= magnitude;
	v[2] /= magnitude;
}
void rotate_view(float view[3], float angle, float x, float y, float z)
{
	float new_x;
	float new_y;
	float new_z;

	float c = cos(angle);
	float s = sin(angle);

	new_x = (x*x*(1 - c) + c)	* view[0];
	new_x += (x*y*(1 - c) - z*s)	* view[1];
	new_x += (x*z*(1 - c) + y*s)	* view[2];

	new_y = (y*x*(1 - c) + z*s)	* view[0];
	new_y += (y*y*(1 - c) + c)	* view[1];
	new_y += (y*z*(1 - c) - x*s)	* view[2];

	new_z = (x*z*(1 - c) - y*s)	* view[0];
	new_z += (y*z*(1 - c) + x*s)	* view[1];
	new_z += (z*z*(1 - c) + c)	* view[2];

	view[0] = new_x;
	view[1] = new_y;
	view[2] = new_z;

	normalize(view);
}

void moveWatch(float rot_x, float rot_y) {
	glPushMatrix();
	glLoadIdentity();
	float rot_axis[3];
	float* c = cameras;
	
	normalize(camview);
	rotate_view(camview, rot_x, 0.0f, 1.0f, 0.0f);
	rot_axis[0] = -camview[2];
	rot_axis[1] = 0.0f;
	rot_axis[2] = camview[0];

	normalize(rot_axis);

	rotate_view(camview, -rot_y, rot_axis[0], rot_axis[1], rot_axis[2]);

	gluLookAt(c[0], c[1], c[2], c[0] + camview[0], c[1] + camview[1], c[2] + camview[2], c[6], c[7], c[8]);
	glGetFloatv(GL_MODELVIEW_MATRIX, wld2cam.matrix());
	cam2wld.set(wld2cam.inverse().m);

	glPopMatrix();
	glutPostRedisplay();
}

void InitTV() {
	TV = new WaveFrontOBJ("tv.obj");

	glPushMatrix();											// Push the current matrix of GL into stack.
	glLoadIdentity();										// Set the GL matrix Identity matrix.
	glTranslatef(0, 0, 0);					// Set the location of cow.
											// Set the direction of cow. These information are stored in the matrix of GL.
	glGetFloatv(GL_MODELVIEW_MATRIX, tv2wld.matrix());		// Read the modelview matrix about location and direction set above, and store it in cow2wld matrix.
	glPopMatrix();											// Pop the matrix on stack to GL.

}

void drawTV()
{
	glPushMatrix();											// Push the current matrix of GL into stack. This is because the matrix of GL will be change while drawing cow.
															// The information about location of cow to be drawn is stored in cow2wld matrix.
															// If you change the value of the cow2wld matrix or the current matrix, cow would rotate or move.
	glMultMatrixf(tv2wld.matrix());

	if (selectMode == 0)									// selectMode == 1 means backbuffer mode.
	{
		drawAxis(5);										// Draw x, y, and z axis.
		glEnable(GL_TEXTURE_2D);
	}
	else
	{
		float r, g, b;
		glDisable(GL_LIGHTING);								// Disable lighting in backbuffer mode.
		glDisable(GL_TEXTURE_2D);
		munge(34, r, g, b);									// Match the corresponding constant color to r, g, b. You can change the color of camera on backbuffer
		glColor3f(r, g, b);
	}
	glBindTexture(GL_TEXTURE_2D, tvPhoto);

	TV->Draw();

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();											// Pop the matrix in stack to GL. Change it the matrix before drawing cow.
}
void InitPlant1() {
	plt1 = new WaveFrontOBJ("plant1.obj");

	glPushMatrix();											// Push the current matrix of GL into stack.
	glLoadIdentity();										// Set the GL matrix Identity matrix.
	glTranslatef(0, 0, 0);					// Set the location of cow.
											// Set the direction of cow. These information are stored in the matrix of GL.
	glGetFloatv(GL_MODELVIEW_MATRIX, plt12wld.matrix());		// Read the modelview matrix about location and direction set above, and store it in cow2wld matrix.
	glPopMatrix();											// Pop the matrix on stack to GL.

}

void drawPlant1()
{
	glPushMatrix();											// Push the current matrix of GL into stack. This is because the matrix of GL will be change while drawing cow.
															// The information about location of cow to be drawn is stored in cow2wld matrix.
															// If you change the value of the cow2wld matrix or the current matrix, cow would rotate or move.
	glMultMatrixf(plt12wld.matrix());

	if (selectMode == 0)									// selectMode == 1 means backbuffer mode.
	{
		drawAxis(5);										// Draw x, y, and z axis.
		glEnable(GL_TEXTURE_2D);
	}
	else
	{
		float r, g, b;
		glDisable(GL_LIGHTING);								// Disable lighting in backbuffer mode.
		glDisable(GL_TEXTURE_2D);
		munge(34, r, g, b);									// Match the corresponding constant color to r, g, b. You can change the color of camera on backbuffer
		glColor3f(r, g, b);
	}
	glBindTexture(GL_TEXTURE_2D, plt1Photo);

	plt1->Draw();
	
	glDisable(GL_TEXTURE_2D);

	glPopMatrix();											// Pop the matrix in stack to GL. Change it the matrix before drawing cow.
}

void InitPlant2() {
	plt2 = new WaveFrontOBJ("plant2.obj");

	glPushMatrix();											// Push the current matrix of GL into stack.
	glLoadIdentity();										// Set the GL matrix Identity matrix.
	glTranslatef(0, 0, 0);					// Set the location of cow.
											// Set the direction of cow. These information are stored in the matrix of GL.
	glGetFloatv(GL_MODELVIEW_MATRIX, plt22wld.matrix());		// Read the modelview matrix about location and direction set above, and store it in cow2wld matrix.
	glPopMatrix();											// Pop the matrix on stack to GL.

}

void drawPlant2()
{
	glPushMatrix();											// Push the current matrix of GL into stack. This is because the matrix of GL will be change while drawing cow.
															// The information about location of cow to be drawn is stored in cow2wld matrix.
															// If you change the value of the cow2wld matrix or the current matrix, cow would rotate or move.
	glMultMatrixf(plt22wld.matrix());

	if (selectMode == 0)									// selectMode == 1 means backbuffer mode.
	{
		drawAxis(5);										// Draw x, y, and z axis.
		glEnable(GL_TEXTURE_2D);
	}
	else
	{
		float r, g, b;
		glDisable(GL_LIGHTING);								// Disable lighting in backbuffer mode.
		glDisable(GL_TEXTURE_2D);
		munge(34, r, g, b);									// Match the corresponding constant color to r, g, b. You can change the color of camera on backbuffer
		glColor3f(r, g, b);
	}
	glBindTexture(GL_TEXTURE_2D, plt2Photo);

	plt2->Draw();

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();											// Pop the matrix in stack to GL. Change it the matrix before drawing cow.
}

void InitPlant3() {
	plt3 = new WaveFrontOBJ("plant3.obj");

	glPushMatrix();											// Push the current matrix of GL into stack.
	glLoadIdentity();										// Set the GL matrix Identity matrix.
	glTranslatef(0, 0, 0);					// Set the location of cow.
											// Set the direction of cow. These information are stored in the matrix of GL.
	glGetFloatv(GL_MODELVIEW_MATRIX, plt32wld.matrix());		// Read the modelview matrix about location and direction set above, and store it in cow2wld matrix.
	glPopMatrix();											// Pop the matrix on stack to GL.

}

void drawPlant3()
{
	glPushMatrix();											// Push the current matrix of GL into stack. This is because the matrix of GL will be change while drawing cow.
															// The information about location of cow to be drawn is stored in cow2wld matrix.
															// If you change the value of the cow2wld matrix or the current matrix, cow would rotate or move.
	glMultMatrixf(plt32wld.matrix());

	if (selectMode == 0)									// selectMode == 1 means backbuffer mode.
	{
		drawAxis(5);										// Draw x, y, and z axis.
		glEnable(GL_TEXTURE_2D);
	}
	else
	{
		float r, g, b;
		glDisable(GL_LIGHTING);								// Disable lighting in backbuffer mode.
		glDisable(GL_TEXTURE_2D);
		munge(34, r, g, b);									// Match the corresponding constant color to r, g, b. You can change the color of camera on backbuffer
		glColor3f(r, g, b);
	}
	glBindTexture(GL_TEXTURE_2D, plt3Photo);

	plt3->Draw();

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();											// Pop the matrix in stack to GL. Change it the matrix before drawing cow.
}

void InitBong() {
	bong = new WaveFrontOBJ("bong_final.obj");

	glPushMatrix();											// Push the current matrix of GL into stack.
	glLoadIdentity();										// Set the GL matrix Identity matrix.
	glTranslatef(0, 1.5, 0);
	glGetFloatv(GL_MODELVIEW_MATRIX, bong2wld.matrix());		// Read the modelview matrix about location and direction set above, and store it in cow2wld matrix.
	glPopMatrix();											// Pop the matrix on stack to GL.

}

void drawBong()
{
	glPushMatrix();											// Push the current matrix of GL into stack. This is because the matrix of GL will be change while drawing cow.
															// The information about location of cow to be drawn is stored in cow2wld matrix.
															// If you change the value of the cow2wld matrix or the current matrix, cow would rotate or move.
	glMultMatrixf(bong2wld.matrix());

	if (selectMode == 0)									// selectMode == 1 means backbuffer mode.
	{
		drawAxis(5);										// Draw x, y, and z axis.
	}
	else
	{
		float r, g, b;
		glDisable(GL_LIGHTING);								// Disable lighting in backbuffer mode.
		munge(31, r, g, b);									// Match the corresponding constant color to r, g, b. You can change the color of camera on backbuffer
		glColor3f(r, g, b);
	}
	bong->Draw();
	/*******************************************************************/
	//(PA #4) : cow object의 normal을 그리는 함수를 추가하십시오.
	/*******************************************************************/

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();											// Pop the matrix in stack to GL. Change it the matrix before drawing cow.
}

void InitMoon() {
	moon = new WaveFrontOBJ("moon.obj");

	glPushMatrix();											// Push the current matrix of GL into stack.
	glLoadIdentity();										// Set the GL matrix Identity matrix.
	//glTranslatef(0, 0, 0);					// Set the location of cow.
											// Set the direction of cow. These information are stored in the matrix of GL.
	glGetFloatv(GL_MODELVIEW_MATRIX, moon2wld.matrix());		// Read the modelview matrix about location and direction set above, and store it in cow2wld matrix.
	glPopMatrix();											// Pop the matrix on stack to GL.

}

void drawMoon()
{
	glPushMatrix();											// Push the current matrix of GL into stack. This is because the matrix of GL will be change while drawing cow.
															// The information about location of cow to be drawn is stored in cow2wld matrix.
															// If you change the value of the cow2wld matrix or the current matrix, cow would rotate or move.
	glMultMatrixf(moon2wld.matrix());

	if (selectMode == 0)									// selectMode == 1 means backbuffer mode.
	{
		drawAxis(5);										// Draw x, y, and z axis.
		glEnable(GL_TEXTURE_2D);
	}
	else
	{
		float r, g, b;
		glDisable(GL_LIGHTING);								// Disable lighting in backbuffer mode.
		glDisable(GL_TEXTURE_2D);
		munge(34, r, g, b);									// Match the corresponding constant color to r, g, b. You can change the color of camera on backbuffer
		glColor3f(r, g, b);
	}
	glBindTexture(GL_TEXTURE_2D, MoonPhoto);

	moon->Draw();
	/*******************************************************************/
	//(PA #4) : cow object의 normal을 그리는 함수를 추가하십시오.
	/*******************************************************************/
	//ctn->Draw_FN();
	//ctn->Draw_VN();
	glDisable(GL_TEXTURE_2D);

	glPopMatrix();											// Pop the matrix in stack to GL. Change it the matrix before drawing cow.
}
void InitSky() {
	sky = new WaveFrontOBJ("skymap_final.obj");

	glPushMatrix();											// Push the current matrix of GL into stack.
	glLoadIdentity();										// Set the GL matrix Identity matrix.
	glTranslatef(0, 0, 0);					// Set the location of cow.
											// Set the direction of cow. These information are stored in the matrix of GL.
	glGetFloatv(GL_MODELVIEW_MATRIX, sky2wld.matrix());		// Read the modelview matrix about location and direction set above, and store it in cow2wld matrix.
	glPopMatrix();											// Pop the matrix on stack to GL.

}

void drawSky()
{
	glPushMatrix();											// Push the current matrix of GL into stack. This is because the matrix of GL will be change while drawing cow.
															// The information about location of cow to be drawn is stored in cow2wld matrix.
															// If you change the value of the cow2wld matrix or the current matrix, cow would rotate or move.
	glMultMatrixf(sky2wld.matrix());

	if (selectMode == 0)									// selectMode == 1 means backbuffer mode.
	{
		drawAxis(5);										// Draw x, y, and z axis.
		glEnable(GL_TEXTURE_2D);
	}
	else
	{
		float r, g, b;
		glDisable(GL_LIGHTING);								// Disable lighting in backbuffer mode.
		glDisable(GL_TEXTURE_2D);
		munge(34, r, g, b);									// Match the corresponding constant color to r, g, b. You can change the color of camera on backbuffer
		glColor3f(r, g, b);
	}
	glBindTexture(GL_TEXTURE_2D, SkyPhoto);

	sky->Draw();
	/*******************************************************************/
	//(PA #4) : cow object의 normal을 그리는 함수를 추가하십시오.
	/*******************************************************************/
	//ctn->Draw_FN();
	//ctn->Draw_VN();
	glDisable(GL_TEXTURE_2D);

	glPopMatrix();											// Pop the matrix in stack to GL. Change it the matrix before drawing cow.
}
void InitRoom() {
	room = new WaveFrontOBJ("room_sm.obj");
	
	glPushMatrix();											// Push the current matrix of GL into stack.
	glLoadIdentity();										// Set the GL matrix Identity matrix.
	glTranslatef(0, 0, 0);					// Set the location of cow.
																			// Set the direction of cow. These information are stored in the matrix of GL.
	glGetFloatv(GL_MODELVIEW_MATRIX, room2wld.matrix());		// Read the modelview matrix about location and direction set above, and store it in cow2wld matrix.
	glPopMatrix();											// Pop the matrix on stack to GL.

}

void drawRoom()
{
	glPushMatrix();											// Push the current matrix of GL into stack. This is because the matrix of GL will be change while drawing cow.
															// The information about location of cow to be drawn is stored in cow2wld matrix.
															// If you change the value of the cow2wld matrix or the current matrix, cow would rotate or move.
	glMultMatrixf(room2wld.matrix());

	if (selectMode == 0)									// selectMode == 1 means backbuffer mode.
	{
		drawAxis(5);										// Draw x, y, and z axis.
		glEnable(GL_TEXTURE_2D);
	}
	else
	{
		float r, g, b;
		glDisable(GL_LIGHTING);								// Disable lighting in backbuffer mode.
		glDisable(GL_TEXTURE_2D);
		munge(34, r, g, b);									// Match the corresponding constant color to r, g, b. You can change the color of camera on backbuffer
		glColor3f(r, g, b);
	}
	glBindTexture(GL_TEXTURE_2D, RoomPhoto);

	room->Draw();
	/*******************************************************************/
	//(PA #4) : cow object의 normal을 그리는 함수를 추가하십시오.
	/*******************************************************************/
	//ctn->Draw_FN();
	//ctn->Draw_VN();
	glDisable(GL_TEXTURE_2D);

	glPopMatrix();											// Pop the matrix in stack to GL. Change it the matrix before drawing cow.
}

void InitCtn() {
	ctn = new WaveFrontOBJ("curtain_1.obj");
	cloth = new mass_cloth();

	cloth->dist_coef = 100;
	cloth->shear_coef = 500;

	cloth->iteration_n = 10;

	cloth->init(ctn);
	
	glPushMatrix();											// Push the current matrix of GL into stack.
	glLoadIdentity();										// Set the GL matrix Identity matrix.
	//glTranslatef(-ctn->bbmax.pos.x, 0, ctn->bbmax.pos.z);					// Set the location of cow.
															// Set the direction of cow. These information are stored in the matrix of GL.
	glGetFloatv(GL_MODELVIEW_MATRIX, ctn2wld.matrix());		// Read the modelview matrix about location and direction set above, and store it in cow2wld matrix.
	glPopMatrix();											// Pop the matrix on stack to GL.

}

void drawCtn()
{
	glPushMatrix();											// Push the current matrix of GL into stack. This is because the matrix of GL will be change while drawing cow.
															// The information about location of cow to be drawn is stored in cow2wld matrix.
															// If you change the value of the cow2wld matrix or the current matrix, cow would rotate or move.
	glMultMatrixf(ctn2wld.matrix());
	
	cloth->sync(ctn);
	for (int i = 0; i < ctn->faces.size(); i++) {
		ctn->faces[i].normal = ctn->faceNormal(ctn->verts[ctn->vIndex[ctn->faces[i].vIndexStart]], ctn->verts[ctn->vIndex[ctn->faces[i].vIndexStart + 1]], ctn->verts[ctn->vIndex[ctn->faces[i].vIndexStart + 2]]);
	}
	ctn->vertexNormal();
	ctn->computeBoundingBox();
	cloth2->ctnbbmax = ctn->bbmax;
	cloth2->ctnbbmin = ctn->bbmin;

	if (selectMode == 0)									// selectMode == 1 means backbuffer mode.
	{
		drawAxis(5);										// Draw x, y, and z axis.
		glEnable(GL_TEXTURE_2D);
	}
	else
	{
		float r, g, b;
		glDisable(GL_LIGHTING);								// Disable lighting in backbuffer mode.
		glDisable(GL_TEXTURE_2D);
		munge(32, r, g, b);									// Match the corresponding constant color to r, g, b. You can change the color of camera on backbuffer
		glColor3f(r, g, b);
	}
	glBindTexture(GL_TEXTURE_2D, photo);
	
	ctn->Draw();
	/*******************************************************************/
	//(PA #4) : cow object의 normal을 그리는 함수를 추가하십시오.
	/*******************************************************************/
	//ctn->Draw_FN();
	//ctn->Draw_VN();
	glDisable(GL_TEXTURE_2D);

	glPopMatrix();											// Pop the matrix in stack to GL. Change it the matrix before drawing cow.
}

void InitCtn2() {
	ctn2 = new WaveFrontOBJ("curtain_2.obj");
	cloth2 = new mass_cloth();

	cloth2->dist_coef = 100;
	cloth2->shear_coef = 500;

	cloth2->iteration_n = 10;

	cloth2->init(ctn2);
	cloth->ctnbbmax = ctn2->bbmax;
	cloth->ctnbbmin = ctn2->bbmin;
	cloth2->ctnbbmax = ctn->bbmax;
	cloth2->ctnbbmin = ctn->bbmin;
	glPushMatrix();											// Push the current matrix of GL into stack.
	glLoadIdentity();										// Set the GL matrix Identity matrix.
	//glTranslatef(ctn2->bbmax.pos.x, 0, -ctn2->bbmax.pos.z);					// Set the location of cow.
															// Set the direction of cow. These information are stored in the matrix of GL.
	glGetFloatv(GL_MODELVIEW_MATRIX, ctn22wld.matrix());		// Read the modelview matrix about location and direction set above, and store it in cow2wld matrix.
	glPopMatrix();											// Pop the matrix on stack to GL.

}

void drawCtn2()
{
	glPushMatrix();											// Push the current matrix of GL into stack. This is because the matrix of GL will be change while drawing cow.
															// The information about location of cow to be drawn is stored in cow2wld matrix.
															// If you change the value of the cow2wld matrix or the current matrix, cow would rotate or move.
	glMultMatrixf(ctn22wld.matrix());

	cloth2->sync(ctn2);
	for (int i = 0; i < ctn2->faces.size(); i++) {
		ctn2->faces[i].normal = ctn2->faceNormal(ctn2->verts[ctn2->vIndex[ctn2->faces[i].vIndexStart]], ctn2->verts[ctn2->vIndex[ctn2->faces[i].vIndexStart + 1]], ctn2->verts[ctn2->vIndex[ctn2->faces[i].vIndexStart + 2]]);
	}
	ctn2->vertexNormal();
	ctn2->computeBoundingBox();
	cloth->ctnbbmax = ctn2->bbmax;
	cloth->ctnbbmin = ctn2->bbmin;

	if (selectMode == 0)									// selectMode == 1 means backbuffer mode.
	{
		drawAxis(5);										// Draw x, y, and z axis.
		glEnable(GL_TEXTURE_2D);
	}
	else
	{
		float r, g, b;
		glDisable(GL_LIGHTING);								// Disable lighting in backbuffer mode.
		glDisable(GL_TEXTURE_2D);
		munge(33, r, g, b);									// Match the corresponding constant color to r, g, b. You can change the color of camera on backbuffer
		glColor3f(r, g, b);
	}
	glBindTexture(GL_TEXTURE_2D, photo2);
	
	ctn2->Draw();
	/*******************************************************************/
	//(PA #4) : cow object의 normal을 그리는 함수를 추가하십시오.
	/*******************************************************************/
	//ctn->Draw_FN();
	//ctn->Draw_VN();
	glDisable(GL_TEXTURE_2D);

	glPopMatrix();											// Pop the matrix in stack to GL. Change it the matrix before drawing cow.
}

void Lighting()
{
	/*******************************************************************/
	//(PA #4) : 다양한 광원을 구현하십시오.
	//1. Point light / Directional light / Spotlight를 서로 다른 색으로 구현하십시오.
	//2. 광원의 위치를 구(sphere)로 표현하십시오.
	//3. Directional light / Spotlight의 경우 빛의 진행방향을 선분으로 표현하십시오.
	/*******************************************************************/
	if (point) { // red color point light
		float light_pos[] = { 0.0, 97.0, -0.5, 1.0 }; // point light position
		float light_ambient[] = { 0.3, 0.3, 0.3, 1.0 }; // point light ambient
		float light_diffuse[] = { 0.5, 0.5, 0.5, 1.0 }; // point light diffuse
		float light_specular[] = { 1.0, 1.0, 1.0, 1.0 }; // point light specular
		glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
		glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
		glLightfv(GL_LIGHT1, GL_POSITION, light_pos);
		glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.8);// set attenuation as linear
		//glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.1);
		//glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 1.0);
		glEnable(GL_LIGHT1); // on point light
		glPushMatrix();
		float emission[] = { 1.0, 1.0, 1.0, 1.0 };// sphere emission
		float emission2[] = { 0.0, 0.0, 0.0, 0.0 };// emission 초기화용 배열
		glMaterialfv(GL_FRONT, GL_EMISSION, emission); // sphere emission setting
		glTranslatef(light_pos[0], light_pos[1], light_pos[2]);
		glutSolidSphere(1.0, 5, 5); // draw sphere
		glMaterialfv(GL_FRONT, GL_EMISSION, emission2);// emission 초기화

		glPopMatrix();
	}
	else {
		glDisable(GL_LIGHT1);// point light off
	}
	if (direc) {
		float light_pos[] = { 2.112, 73.223, 115.538, 0.0 };// directional light position
		float light_dir[] = { -2.112, -73.223, -115.538 }; //directional light direction
		float light_ambient[] = { 0.6, 0.6, 0.3, 1.0 };// directional light ambient
		float light_diffuse[] = { 0.2, 0.2, 0.1, 1.0 };// directional light diffuse
		float light_specular[] = { 0.1, 0.1, 0.05, 1.0 };// directional light specular
		glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient); //setting light
		glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse);
		glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular);
		glLightfv(GL_LIGHT2, GL_POSITION, light_pos);
		glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, light_dir);
		glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 1.0);// set attenuation as linear
		glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 1.0);
		glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 2.0);
		glEnable(GL_LIGHT2); // enable directional light
	}
	else {
		glDisable(GL_LIGHT2);//directional light off
	}
	if (spot) { // green and blue color spot light
		float light_pos[] = { 10.0, 5.0, 0.0, 1.0 }; // light position
		float light_dir[] = { -1.0, 0.0, 0.0 }; //light direction
		float light_ambient[] = { 0.0, 0.5, 0.5, 1.0 };
		float light_diffuse[] = { 0.0, 0.5, 0.5, 1.0 };
		float light_specular[] = { 0.0, 1.0, 1.0, 1.0 };
		glLightfv(GL_LIGHT3, GL_POSITION, light_pos);
		glLightfv(GL_LIGHT3, GL_AMBIENT, light_ambient);
		glLightfv(GL_LIGHT3, GL_DIFFUSE, light_diffuse);
		glLightfv(GL_LIGHT3, GL_SPECULAR, light_specular);
		glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, 50.0);
		glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, 50.0);
		glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, light_dir);
		glLightf(GL_LIGHT3, GL_CONSTANT_ATTENUATION, 0.0);// set attenuation as linear
		glLightf(GL_LIGHT3, GL_LINEAR_ATTENUATION, 0.1);
		glEnable(GL_LIGHT3);
		glPushMatrix();
		float emission[] = { 0.0, 1.0, 1.0, 1.0 };
		float emission2[] = { 0.0, 0.0, 0.0, 0.0 };
		glMaterialfv(GL_FRONT, GL_EMISSION, emission);
		glTranslatef(light_pos[0], light_pos[1], light_pos[2]);
		glutSolidSphere(1.0, 5, 5);
		glMaterialfv(GL_FRONT, GL_EMISSION, emission2);
		glBegin(GL_LINES);			// Start drawing lines.
		glColor3d(0, 1, 1);			// line color is green and blue.
		glVertex3d(0, 0, 0);
		glVertex3d(2.0*light_dir[0], 2.0*light_dir[1], 2.0*light_dir[2]);
		glEnd();
		glPopMatrix();
	}
	else {
		glDisable(GL_LIGHT3); // spotlight off
	}
}

/*********************************************************************************
* Call this part whenever display events are needed.
* Display events are called in case of re-rendering by OS. ex) screen movement, screen maximization, etc.
* Or, user can occur the events by using glutPostRedisplay() function directly.
* this part is called in main() function by registering on glutDisplayFunc(display).
**********************************************************************************/
void display()
{
	glLoadMatrixf(wld2cam.matrix());
	if (selectMode == 0)
		glClearColor(0, 0.6, 0.8, 1);								// Clear color setting
	else
		glClearColor(0, 0, 0, 1);									// When the backbuffer mode, clear color is set to black

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// Clear the screen

	if (shademodel)
		glShadeModel(GL_FLAT);										// Set Flat Shading
	else
		glShadeModel(GL_SMOOTH);

	moveCam();
	moveCtn();
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	//drawSky();

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	drawSky();
	drawRoom();
	drawMoon();
	drawCtn();
	drawCtn2();
	drawPlant1();
	drawPlant2();
	drawPlant3();
	drawBong();
	drawTV();
	Lighting();
	TVon();


	glFlush();

	// If it is not backbuffer mode, swap the screen. In backbuffer mode, this is not necessary because it is not presented on screen.
	if (selectMode == 0)
		glutSwapBuffers();

	frame++;
}





/*********************************************************************************
* Call this part whenever size of the window is changed.
* This part is called in main() function by registering on glutReshapeFunc(reshape).
**********************************************************************************/
void reshape(int w, int h)
{
	width = w;
	height = h;
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);            // Select The Projection Matrix
	glLoadIdentity();                       // Reset The Projection Matrix
	float aspect = width / float(height);
	gluPerspective(45, aspect, 1, 1024);

	glMatrixMode(GL_MODELVIEW);             // Select The Modelview Matrix
	glLoadIdentity();                       // Reset The Projection Matrix
}





//------------------------------------------------------------------------------
void initialize()
{
	// Set up OpenGL state
	glEnable(GL_DEPTH_TEST);         // Enables Depth Testing
	// Initialize the matrix stacks
	reshape(width, height);
	// Define lighting for the scene
	float light_pos[] = { 2.112, 73.223, 115.538, 0.0 };// directional light position
	float light_dir[] = { -2.112, -73.223, -115.538 }; //directional light direction
	float light_ambient[] = { 0.6, 0.6, 0.3, 1.0 };// directional light ambient
	float light_diffuse[] = { 0.5, 0.5, 0.25, 1.0 };// directional light diffuse
	float light_specular[] = { 0.3, 0.3, 0.15, 1.0 };// directional light specular
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient); //setting light
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_dir);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);// set attenuation as linear
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 1.0);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 2.0);
	glEnable(GL_LIGHT0); // enable directional light
	
	// Texture Mapping
	photo = LoadTexture("curtain3.png");
	photo2 = LoadTexture("curtain3.png");
	RoomPhoto = LoadTexture("room.png");
	SkyPhoto = LoadTexture("nightsky.PNG");
	MoonPhoto = LoadTexture("moon.jpg");
	plt1Photo = LoadTexture("plant1.png");
	plt2Photo = LoadTexture("plant2.png");
	plt3Photo = LoadTexture("plant3.png");
	tvPhoto = LoadTexture("tv.png");


	InitSky();
	InitRoom();
	InitCtn();
	InitCtn2();
	InitMoon();
	InitPlant1();
	InitPlant2();
	InitPlant3();
	InitBong();
	InitTV();
	InitCamera();
	// edited
	
	for (int i = 0; i < cloth->fp.size(); i++)
	{
		spreadCtnLoc1[i].id = i;
		spreadCtnLoc1[i].pos = cloth->fp[i]->position.x;
	}
	for (int i = 0; i < cloth2->fp.size(); i++) {
		spreadCtnLoc2[i].id = i;
		spreadCtnLoc2[i].pos = cloth2->fp[i]->position.x;
	}
	sort(spreadCtnLoc1, spreadCtnLoc1 + cloth->fp.size(), list1());
	sort(spreadCtnLoc2, spreadCtnLoc2 + cloth2->fp.size(), list2());

<<<<<<< HEAD
=======
	// edited
	// ctn 1 : 오른쪽
	// ctn 2 : 왼쪽
	for (int i = 0; i < cloth->fp.size(); i++)
	{
		spreadCtnLoc1[i].id = i;
		spreadCtnLoc1[i].pos = cloth->fp[i]->position.x;
	}
	for (int i = 0; i < cloth2->fp.size(); i++) {
		spreadCtnLoc2[i].id = i;
		spreadCtnLoc2[i].pos = cloth2->fp[i]->position.x;
	}
	sort(spreadCtnLoc1, spreadCtnLoc1 + cloth->fp.size(), list1());
	sort(spreadCtnLoc2, spreadCtnLoc2 + cloth2->fp.size(), list2());

>>>>>>> 4352909dc06d9e6cbbc55ec712f97b6f66dc47fb
	for (int i = 0; i < cloth->fp.size(); i++) {
		foldCtnLoc1[i].id = spreadCtnLoc1[i].id;
		foldCtnLoc1[i].pos = spreadCtnLoc1[0].pos - 1 - i;
	}
	for (int i = 0; i < cloth2->fp.size(); i++) {
		foldCtnLoc2[i].id = spreadCtnLoc2[i].id;
		foldCtnLoc2[i].pos = spreadCtnLoc2[0].pos + 1 + i;
	}
}





/*********************************************************************************
* Call this part whenever mouse button is clicked.
* This part is called in main() function by registering on glutMouseFunc(onMouseButton).
**********************************************************************************/
void onMouseButton(int button, int state, int x, int y)
{
	y = height - y - 1;
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			printf("Left mouse click at (%d, %d)\n", x, y);

			// After drawing object on backbuffer, you can recognize which object is selected by reading pixel of (x, y).
			// Change the value of selectMode to 1, then draw the object on backbuffer when display() function is called. 
			selectMode = 1;
			display();
			glReadBuffer(GL_BACK);
			unsigned char pixel[3];
			glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
			int pixel_value = unmunge(pixel[0], pixel[1], pixel[2]);
			printf("pixel = %d\n", pixel_value);

			if (picking) {
				if (pixel_value == 31) current_obj = -1;
				else if (pixel_value == 32) current_obj = 0;
				else if (pixel_value == 33) current_obj = 1;
				else {
					current_obj = 2;
				}
				picking = false; //picking이 종료되었으므로 다시 false로 복구한다.
			}

			selectMode = 0;
			// Save current clicked location of mouse here, and then use this on onMouseDrag function. 
			oldX = x;
			oldY = y;
		}
	}

	glutPostRedisplay();
}



void onMouseDrag(int x, int y)
{	
	y = height - y - 1;
	
	moveX = (x - oldX)*2.0;
	moveY = (y - oldY)*2.0;
	float move[3] = { (moveX * cam2wld.m[0][0] + moveY * cam2wld.m[1][0]), (moveX * cam2wld.m[0][1] + moveY * cam2wld.m[1][1]), (moveX * cam2wld.m[0][2] + moveY * cam2wld.m[1][2]) };
	if (current_obj == 0) {
		cloth->add_force(Vector(move[0], move[1], move[2]));
	}
	else if (current_obj == 1) {
		cloth2->add_force(Vector(move[0], move[1], move[2]));
	}
	if (current_obj == 2) {
		moveWatch(moveX * 0.0005, moveY * 0.0005);
	}
	printf("in drag (%d, %d)\n", x - oldX, y - oldY);
	oldX = x;
	oldY = y;
	glutPostRedisplay();
}





/*********************************************************************************
* Call this part whenever user types keyboard.
* This part is called in main() function by registering on glutKeyboardFunc(onKeyPress).
**********************************************************************************/
void onKeyPress(unsigned char key, int x, int y)
{
	
	/*******************************************************************/
	//(PA #2,#3)  수행한 내용 추가
	/*******************************************************************/
	
	if (key == 'r'){
		shademodel = !shademodel;
	}
	if (key == 'p') { //picking 상태 on
		picking = true;
	}
	if (key == 't') {
		tvon = !tvon;
	}

	glutPostRedisplay();
}




void SpecialKey(int key, int x, int y)
{
	switch (key) { //만약 각각의 light가 on에서 off 되는 상황이라면 해당 light를 disable해주어야 한다.
	case GLUT_KEY_F1:	if (point) {
							point = !point;
							//glDisable(GL_LIGHT1);
						}
						else point = !point; 
						break;
	case GLUT_KEY_F2:	if (direc) {
							direc = !direc;
							//glDisable(GL_LIGHT2);
						}
						else direc = !direc; 
						break;
	case GLUT_KEY_F3:	if (spot) {
							spot = !spot;
							//glDisable(GL_LIGHT3);
						}
						else spot = !spot; 
						break;
	}

	glutPostRedisplay();
}



void idle() {
	Vector gravity(0.0, -20.0, 0.0);
	float dt = 0.01;

	cloth->integrate(dt, gravity);
	cloth2->integrate(dt, gravity);
	glutPostRedisplay();
}


//------------------------------------------------------------------------------
void main(int argc, char* argv[])
{
	width = 800;
	height = 600;
	frame = 0;
	glutInit(&argc, argv); // Initialize openGL.
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);	// Initialize display mode. This project will use float buffer and RGB color.
	glutInitWindowSize(width, height);				// Initialize window size.
	glutInitWindowPosition(100, 100);				// Initialize window coordinate.
	glutCreateWindow("PA4");

	glutDisplayFunc(display);						// Register display function to call that when drawing screen event is needed.
	glutReshapeFunc(reshape);						// Register reshape function to call that when size of the window is changed.
	glutKeyboardFunc(onKeyPress);					// Register onKeyPress function to call that when user presses the keyboard.
	glutMouseFunc(onMouseButton);					// Register onMouseButton function to call that when user moves mouse.
	glutMotionFunc(onMouseDrag);					// Register onMouseDrag function to call that when user drags mouse.

	glutIdleFunc(idle);

	glutSpecialFunc(SpecialKey);

	initialize();									// Initialize the other thing.
	
	glutMainLoop();									// Execute the loop which handles events.
}