#include "stdafx.h"
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <iostream>

float sealevel;
float polysize;
double eyePosX;
double eyePosY;
double eyePosZ;
double centerX = 0;
double centerY = 0;
double centerZ = 0;
double upX = 0;
double upY = 0;
double upZ = 1;



double crvX;
double crvY;
double crvZ;

double translateX;
double translateY;
double translateZ;

float angle;
float xAxis;
float yAxis;
float zAxis;

bool rotateLeft;
bool rotateRight;
bool rotateUp;
bool rotateDown;



void interVecCalc(){

	double rvX;
	double rvY;
	double rvZ;

	double CAx = eyePosX - eyePosX;
	double CAy = eyePosY - eyePosY;
	double CAz = eyePosZ - 0;

	double ABx = eyePosX - centerX;
	double ABy = eyePosY - centerY;
	double ABz = eyePosZ - centerZ;

	double AB2x = eyePosX - centerX;
	double AB2y = eyePosY - centerY;
	double AB2z = eyePosZ - centerZ;

	double ABMagS = pow(sqrt(pow(ABx, 2)+ pow(ABy, 2)+ pow(ABz, 2)), 2);

	double dotOverMagS = (ABx*CAx + ABy*CAy + ABz*CAz) / ABMagS;

	AB2x *= dotOverMagS;
	AB2y *= dotOverMagS;
	AB2z *= dotOverMagS;

	rvX = CAx - AB2x;
	rvY = CAy - AB2y;
	rvZ = CAz - AB2z;

	crvX = ABy*rvZ - ABz*rvY;
	crvY = ABz*rvX - ABx*rvZ;
	crvZ = ABx*rvY - ABy*rvX;

}

int seed(float x, float y) {
	static int a = 1588635695, b = 1117695901;
	int xi = *(int *)&x;
	int yi = *(int *)&y;
	return ((xi * a) % b) - ((yi * b) % a);
}

void mountain(float x0, float y0, float z0, float x1, float y1, float z1, float x2, float y2, float z2, float s)
{
	float x01, y01, z01, x12, y12, z12, x20, y20, z20;

	if (s < polysize) {
		x01 = x1 - x0;
		y01 = y1 - y0;
		z01 = z1 - z0;

		x12 = x2 - x1;
		y12 = y2 - y1;
		z12 = z2 - z1;

		x20 = x0 - x2;
		y20 = y0 - y2;
		z20 = z0 - z2;

		float nx = y01*(-z20) - (-y20)*z01;
		float ny = z01*(-x20) - (-z20)*x01;
		float nz = x01*(-y20) - (-x20)*y01;

		float den = sqrt(nx*nx + ny*ny + nz*nz);

		if (den > 0.0) {
			nx /= den;
			ny /= den;
			nz /= den;
		}

		glNormal3f(nx, ny, nz);
		glBegin(GL_TRIANGLES);
		glVertex3f(x0, y0, z0);
		glVertex3f(x1, y1, z1);
		glVertex3f(x2, y2, z2);
		glEnd();

		return;
	}

	x01 = 0.5*(x0 + x1);
	y01 = 0.5*(y0 + y1);
	z01 = 0.5*(z0 + z1);

	x12 = 0.5*(x1 + x2);
	y12 = 0.5*(y1 + y2);
	z12 = 0.5*(z1 + z2);

	x20 = 0.5*(x2 + x0);
	y20 = 0.5*(y2 + y0);
	z20 = 0.5*(z2 + z0);

	s *= 0.5;

	srand(seed(x01, y01));
	z01 += 0.3*s*(2.0*((float)rand() / (float)RAND_MAX) - 1.0);
	srand(seed(x12, y12));
	z12 += 0.3*s*(2.0*((float)rand() / (float)RAND_MAX) - 1.0);
	srand(seed(x20, y20));
	z20 += 0.3*s*(2.0*((float)rand() / (float)RAND_MAX) - 1.0);

	mountain(x0, y0, z0, x01, y01, z01, x20, y20, z20, s);
	mountain(x1, y1, z1, x12, y12, z12, x01, y01, z01, s);
	mountain(x2, y2, z2, x20, y20, z20, x12, y12, z12, s);
	mountain(x01, y01, z01, x12, y12, z12, x20, y20, z20, s);
}

void init(void)
{
	rotateLeft = false;

	eyePosX = 0.5;
	eyePosY = 0;
	eyePosZ = 0.25;

	translateX = -0.25;
	translateY = -0.5;
	translateZ = 0;

	angle = 0;
	
	xAxis = 0;
	yAxis = 0;
	zAxis = 1;

	GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat lpos[] = { 0.0, 1.0, 0.0, 0.0 };

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_POSITION, lpos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);

	glClearColor(0.5, 0.5, 1.0, 0.0);
	/* glShadeModel (GL_FLAT); */
	glEnable(GL_DEPTH_TEST);

	sealevel = 0.0;
	polysize = 0.01;
}

void display(void)
{
	static GLfloat angle = 0.0;
	static GLfloat theta = 0.0;
	static GLfloat thetaP = 0.0;

	GLfloat tanamb[] = { 0.2, 0.15, 0.1, 1.0 };
	GLfloat tandiff[] = { 0.4, 0.3, 0.2, 1.0 };

	GLfloat seaamb[] = { 0.0, 0.0, 0.2, 1.0 };
	GLfloat seadiff[] = { 0.0, 0.0, 0.8, 1.0 };
	GLfloat seaspec[] = { 0.5, 0.5, 1.0, 1.0 };


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);
	glLoadIdentity();             /* clear the matrix */
	/* viewing transformation  */
	
	if (rotateLeft)
	{
		angle += 1;
	}

	else if (rotateRight)
	{
		angle -= 1;
	}
	else if (rotateUp)
	{
		centerZ += 0.01;
	}

	else if (rotateDown)
	{
		centerZ -= 0.01;
	}

	gluLookAt(eyePosX, eyePosY, eyePosZ, centerX, centerY, centerZ, upX, upY, upZ);

	glRotatef(angle, xAxis, yAxis, zAxis);

	glTranslatef(translateX, translateY, translateZ);      /* modeling transformation */


	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, tanamb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, tandiff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, tandiff);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10.0);

	mountain(0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0);
	mountain(1.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, seaamb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, seadiff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, seaspec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10.0);

	glNormal3f(0.0, 0.0, 1.0);
	glBegin(GL_QUADS);
	glVertex3f(0.0, 0.0, sealevel);
	glVertex3f(1.0, 0.0, sealevel);
	glVertex3f(1.0, 1.0, sealevel);
	glVertex3f(0.0, 1.0, sealevel);
	glEnd();

	glutSwapBuffers();
	glFlush();

	glutPostRedisplay();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0, 1.0, 0.01, 10.0);
	glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case '-':
		sealevel -= 0.01;
		break;
	case '+':
	case '=':
		sealevel += 0.01;
		break;
	case 'f':
		polysize *= 0.5;
		break;
	case 'c':
		polysize *= 2.0;
		break;
	case 'a':
		xAxis = eyePosX;
		yAxis = eyePosY;
		zAxis = eyePosZ;
		rotateLeft = true;
		break;
	case 'w':
		rotateUp = true;
		break;
	case 's':
		rotateDown = true;
		break;
	case 'd':
		xAxis = eyePosX;
		yAxis = eyePosY;
		zAxis = eyePosZ;
		rotateRight = true;
		break;
	case 27:
		exit(0);
		break;
	default:
		rotateLeft = false;
	}
}

void timer(int v)
{
	translateX += 0.001;
	glutPostRedisplay(); // trigger display function by sending redraw into message queue
	glutTimerFunc(1000 / 30, timer, v); // restart timer again
}

void keyUp(unsigned char key, int x, int y) {
	if (key == 'a') 
	{
		rotateLeft = false;
	}

	if (key == 'd')
	{
		rotateRight = false;
	}

	if (key == 's')
	{
		rotateDown = false;
	}

	if (key == 'w')
	{
		rotateUp = false;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyUp);
	glutTimerFunc(100, timer, 30);

	glutMainLoop();
	return 0;
}