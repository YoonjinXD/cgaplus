#include <iostream>
#include <fstream>
#include <Windows.h>
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


using namespace std;

// texture mapping
GLuint photo;

// 'cameras' stores infomation of 5 cameras.
float cameras[5][9] =
{
	{ 28, 18, 28, 0, 2, 0, 0, 1, 0 },
	{ 28, 18, -28, 0, 2, 0, 0, 1, 0 },
	{ -28, 18, 28, 0, 2, 0, 0, 1, 0 },
	{ -12, 12, 0, 0, 2, 0, 0, 1, 0 },
	{ 0, 100, 0, 0, 0, 0, 1, 0, 0 }
};
int cameraCount = sizeof(cameras) / sizeof(cameras[0]);

int cameraIndex;
vector<Matrix> wld2cam, cam2wld;

Matrix ctn2wld;
WaveFrontOBJ* ctn;
mass_cloth *cloth;

unsigned floorTexID;

int frame = 0;
int width, height;
int oldX, oldY;

void translate();
void rotate();

int selectMode;
bool picking = false;

/*******************************************************************/
//(PA #2,#3) 수행한 내용 추가
/*******************************************************************/
float dist_x; //x축 마우스 드래그 정도
float dist_y; //y축 마우스 드래그 정도
float dist; //마우스 드래그 정도를 계산한 값 저장
int axis = 0; //현재 선택된 축을 저장. 0,1,2 순서대로 x,y,z
bool spin = false; //rotate의 on/off를 결정해주는 변수.
int sp_mode = 0; //space mode. 현재 선택된 mode가 viewing인지 modeling인지 저장. 초기값은 modeling space
int current_obj = 0; //현재 선택된 object를 저장. 0=cow, 1=bunny. 초기값은 cow

					 /*PA4 추가 변수*/
bool drawF = false; //Face Normal on/off
bool drawN = false; //Vertex Normal on/off
bool point = false; //point light on/off
bool direc = false; //direction light on/off
bool spot = false; //spot light on/off

bool shademodel = false;

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





void InitCamera() {

	// initialize camera frame transforms.
	for (int i = 0; i < cameraCount; i++)
	{
		float* c = cameras[i];													// 'c' points the coordinate of i-th camera.
		wld2cam.push_back(Matrix());											// Insert {0} matrix to wld2cam vector.
		glPushMatrix();															// Push the current matrix of GL into stack.

		glLoadIdentity();														// Set the GL matrix Identity matrix.
		gluLookAt(c[0], c[1], c[2], c[3], c[4], c[5], c[6], c[7], c[8]);		// Setting the coordinate of camera.
		glGetFloatv(GL_MODELVIEW_MATRIX, wld2cam[i].matrix());					// Read the world-to-camera matrix computed by gluLookAt.
		cam2wld.push_back(wld2cam[i].inverse());								// Get the camera-to-world matrix.

		glPopMatrix();															// Transfer the matrix that was pushed the stack to GL.
	}
	cameraIndex = 0;
}





void drawCamera()
{
	int i;
	// set viewing transformation.
	glLoadMatrixf(wld2cam[cameraIndex].matrix());

	// draw cameras.
	for (i = 0; i < (int)wld2cam.size(); i++)
	{
		if (i != cameraIndex)
		{
			glPushMatrix();													// Push the current matrix on GL to stack. The matrix is wld2cam[cameraIndex].matrix().
			glMultMatrixf(cam2wld[i].matrix());								// Multiply the matrix to draw i-th camera.
			if (selectMode == 0)											// selectMode == 1 means backbuffer mode.
			{
				drawAxis(5);												// Draw x, y, and z axis.
			}
			else
			{
				float r, g, b;
				glDisable(GL_LIGHTING);										// Disable lighting in backbuffer mode.
				munge(i + 1, r, g, b);										// Match the corresponding (i+1)th color to r, g, b. You can change the color of camera on backbuffer.
				glColor3f(r, g, b);											// Set r, g, b the color of camera.
			}

			glutSolidSphere(1.0, 10, 10);

			glPopMatrix();													// Call the matrix on stack. wld2cam[cameraIndex].matrix() in here.
		}
	}
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

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	stbi_image_free(image);

	return texture;
}

void InitCtn() {
	ctn = new WaveFrontOBJ("plant_test.obj");
	ctn->WaveFrontMTL("plant_test.mtl");
	cloth = new mass_cloth();
	cloth->dx = 0.1;
	cloth->dy = 0.1;
	cloth->dz = 0.1;
	cloth->grid_n = 40;
	cloth->grid_m = 40;
	cloth->grid_l = 40;
	cloth->dist_coef = 2500;
	cloth->shear_coef = 100;

	cloth->iteration_n = 10;

	cloth->init(ctn);
	glPushMatrix();											// Push the current matrix of GL into stack.
	glLoadIdentity();										// Set the GL matrix Identity matrix.
//	glTranslatef(0, ctn->bbmax.pos.y, 0);					// Set the location of cow.
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
	if (selectMode == 0)									// selectMode == 1 means backbuffer mode.
	{
		drawAxis(5);										// Draw x, y, and z axis.
	}
	else
	{
		float r, g, b;
		glDisable(GL_LIGHTING);								// Disable lighting in backbuffer mode.
		munge(32, r, g, b);									// Match the corresponding constant color to r, g, b. You can change the color of camera on backbuffer
		glColor3f(r, g, b);
	}
	/*
	glBindTexture(GL_TEXTURE_2D, photo);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	*/
	ctn->Draw();
//	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	/*******************************************************************/
	//(PA #4) : cow object의 normal을 그리는 함수를 추가하십시오.
	/*******************************************************************/
	//ctn->Draw_FN();
	//ctn->Draw_VN();
	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glPopMatrix();											// Pop the matrix in stack to GL. Change it the matrix before drawing cow.
}

/*********************************************************************************
* Draw floor on 3D plane.
**********************************************************************************/
void drawFloor()
{
	if (frame == 0)
	{
		// Initialization part.
		// After making checker-patterned texture, use this repetitively.

		// Insert color into checker[] according to checker pattern.
		const int size = 8;
		unsigned char checker[size*size * 3];
		for (int i = 0; i < size*size; i++)
		{
			if (((i / size) ^ i) & 1)
			{
				checker[3 * i + 0] = 100;
				checker[3 * i + 1] = 100;
				checker[3 * i + 2] = 100;
			}
			else
			{
				checker[3 * i + 0] = 200;
				checker[3 * i + 1] = 200;
				checker[3 * i + 2] = 200;
			}
		}

		// Make texture which is accessible through floorTexID. 
		glGenTextures(1, &floorTexID);
		glBindTexture(GL_TEXTURE_2D, floorTexID);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, checker);
	}

	glDisable(GL_LIGHTING);

	// Set background color.
	if (selectMode == 0)
		glColor3d(0.35, 0.2, 0.1);
	else
	{
		// In backbuffer mode.
		float r, g, b;
		munge(34, r, g, b);
		glColor3f(r, g, b);
	}

	// Draw background rectangle.
	glBegin(GL_POLYGON);
	glVertex3f(-2000, -0.2, -2000);
	glVertex3f(-2000, -0.2, 2000);
	glVertex3f(2000, -0.2, 2000);
	glVertex3f(2000, -0.2, -2000);
	glEnd();


	// Set color of the floor.
	if (selectMode == 0)
	{
		// Assign checker-patterned texture.
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, floorTexID);
	}
	else
	{
		// Assign color on backbuffer mode.
		float r, g, b;
		munge(35, r, g, b);
		glColor3f(r, g, b);
	}

	// Draw the floor. Match the texture's coordinates and the floor's coordinates resp. 
	glBegin(GL_POLYGON);
	glTexCoord2d(0, 0);
	glVertex3d(-12, -0.1, -12);		// Texture's (0,0) is bound to (-12,-0.1,-12).
	glTexCoord2d(0, 1);
	glVertex3d(-12, -0.1, 12);		// Texture's (0,1) is bound to (-12,-0.1,12).
	glTexCoord2d(1, 1);
	glVertex3d(12, -0.1, 12);		// Texture's (1,1) is bound to (12,-0.1,12).
	glTexCoord2d(1, 0);
	glVertex3d(12, -0.1, -12);		// Texture's (1,0) is bound to (12,-0.1,-12).

	glEnd();

	if (selectMode == 0)
	{
		glDisable(GL_TEXTURE_2D);
		drawAxis(5);				// Draw x, y, and z axis.
	}
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
		float light_pos[] = { 10.0, 10.0, -10.0, 1.0 }; // point light position
		float light_ambient[] = { 0.3, 0.0, 0.0, 1.0 }; // point light ambient
		float light_diffuse[] = { 0.5, 0.0, 0.0, 1.0 }; // point light diffuse
		float light_specular[] = { 1.0, 0.0, 0.0, 1.0 }; // point light specular
		glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
		glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
		glLightfv(GL_LIGHT1, GL_POSITION, light_pos);
		glEnable(GL_LIGHT1); // on point light
		glPushMatrix();
		float emission[] = { 1.0, 0.0, 0.0, 1.0 };// sphere emission
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
	if (direc) {// green color directional light
		float light_pos[] = { -10.0, 10.0, -10.0, 0.0 };// directional light position
		float light_dir[] = { -1.0, 1.0, -1.0 }; //directional light direction
		float light_ambient[] = { 0.0, 0.3, 0.0, 1.0 };// directional light ambient
		float light_diffuse[] = { 0.0, 0.5, 0.0, 1.0 };// directional light diffuse
		float light_specular[] = { 0.0, 1.0, 0.0, 1.0 };// directional light specular
		glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient); //setting light
		glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse);
		glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular);
		glLightfv(GL_LIGHT2, GL_POSITION, light_pos);
		glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, light_dir);
		glEnable(GL_LIGHT2); // enable directional light
		glPushMatrix();
		float emission[] = { 0.0, 1.0, 0.0, 1.0 };
		float emission2[] = { 0.0, 0.0, 0.0, 0.0 };
		glMaterialfv(GL_FRONT, GL_EMISSION, emission);
		glTranslatef(light_pos[0], light_pos[1], light_pos[2]);
		glutSolidSphere(1.0, 5, 5);
		glMaterialfv(GL_FRONT, GL_EMISSION, emission2);
		glBegin(GL_LINES);			// Start drawing lines.
		glColor3d(0, 1, 0);			// line color is red.
		glVertex3d(0, 0, 0);
		glVertex3d(-2.0*light_dir[0], -2.0*light_dir[1], -2.0*light_dir[2]);
		glEnd();
		glPopMatrix();
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
	if (selectMode == 0)
		glClearColor(0, 0.6, 0.8, 1);								// Clear color setting
	else
		glClearColor(0, 0, 0, 1);									// When the backbuffer mode, clear color is set to black

	

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// Clear the screen

	if (shademodel)
		glShadeModel(GL_FLAT);										// Set Flat Shading
	else
		glShadeModel(GL_SMOOTH);

	/*
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 1 - 1.0);
		glVertex3f(0, 10,10);
		glTexCoord2f(0.0, 1 - 0.0);
		glVertex3f(0, 0, 0);
		glTexCoord2f(1.0, 1 - 0.0);
		glVertex3f(0, 0, 10);
		glTexCoord2f(1.0, 1 - 1.0);
		glVertex3f(0, 10, 20);
	glEnd();
	*/

	drawCtn();
	drawCamera();													// and draw all of them.
//	drawFloor();													// Draw floor.

	Lighting();

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
	float light_pos[] = { 1.0, 1.0, 1.0, 0.0 };
	float light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
	float light_diffuse[] = { 0.9, 0.9, 0.9, 1.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glEnable(GL_LIGHT0);

	// Texture Mapping
	photo = LoadTexture("sky_test.jpg");



	InitCtn();
	InitCamera();
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
				if (pixel_value == 32) current_obj = 0;
				else if (pixel_value == 33) current_obj = 1;
				else {
					current_obj = -1;
					printf("Picking Error: Please select objects");
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
	cloth->add_force(Vector((float)(x - oldX), (float)(y - oldY), 0));
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
	//카메라 시점 변경
	if ((key >= '1') && (key <= '5'))
		cameraIndex = key - '1';

	/*******************************************************************/
	//(PA #2,#3)  수행한 내용 추가
	/*******************************************************************/
	if (key == 's') {
		shademodel = !shademodel;
	}
	else if (key == 'm') { //modeling space
		sp_mode = 0;
	}
	else if (key == 'v') { //viewing space
		sp_mode = 1;
	}
	else if (key == 'x') { //x축
		axis = 0;
	}
	else if (key == 'y') { //y축
		axis = 1;
	}
	else if (key == 'z') { //z축
		axis = 2;
	}
	else if (key == 'r') { //rotate 상태 on/off
		if (spin == true) spin = false;
		else spin = true;
	}
	else if (key == 'p') { //picking 상태 on
		picking = true;
	}
	else if (key == 'f') {//Draw Face Normal
		drawF = !drawF;
	}
	else if (key == 'n') {//Draw Vertex Normal
		drawN = !drawN;
	}

	glutPostRedisplay();
}




void SpecialKey(int key, int x, int y)
{
	switch (key) { //만약 각각의 light가 on에서 off 되는 상황이라면 해당 light를 disable해주어야 한다.
	case GLUT_KEY_F1:	if (point) {
		point = !point;
		glDisable(GL_LIGHT1);
	}
						else point = !point;
						break;
	case GLUT_KEY_F2:	if (direc) {
		direc = !direc;
		glDisable(GL_LIGHT2);
	}
						else direc = !direc;
						break;
	case GLUT_KEY_F3:	if (spot) {
		spot = !spot;
		glDisable(GL_LIGHT3);
	}
						else spot = !spot;
						break;
	}

	glutPostRedisplay();
}





void rotate() {
	glPushMatrix();
	glLoadIdentity();
	//나중에 추가
	glPopMatrix();
}





void translate() {
	glPushMatrix();
	glLoadIdentity();
	//나중에 추가
	glPopMatrix();
}





void idle() {
	Vector gravity(0.0, -8.0, 0.0);
	float dt = 0.01;

	//	spring.integrate(dt, gravity);
//	cloth->integrate(dt, gravity);
	glutPostRedisplay();
}


//------------------------------------------------------------------------------
void main(int argc, char* argv[])
{
	width = 800;
	height = 600;
	frame = 0;
	glutInit(&argc, argv); // Initialize openGL.
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);	// Initialize display mode. This project will use float buffer and RGB color.
	glutInitWindowSize(width, height);				// Initialize window size.
	glutInitWindowPosition(100, 100);				// Initialize window coordinate.
	glutCreateWindow("Texture Mapping");
	
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