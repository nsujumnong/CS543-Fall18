// This is a modified code based on OpenGL starter code

// Homework 1: Nuttaworn Sujumnong

// Sierpinski Gasket code based on: Interactive Computer Graphics 6th edition
// Koch Snowflake code based on: Computer Graphics using OpenGL 2nd edition

#include "Angel.h"
#include <GL/glew.h>
#include <glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include "math.h"
#include <cmath>

const int NumPoints = 5000;
mat4 orth = Ortho2D(0, 640.0, 0, 480.0);
const float width = 640;
const float height = 480;


// remember to prototype
void initGPUBuffers(void);
void shaderSetup(void);
void displayTriangle(void);
void keyboard(unsigned char key, int x, int y);

typedef vec2 point2;

using namespace std;

GLuint program;

const int NumTimesToSubdivide = 5;
const int NumTriangles = 243;  // 3^5 triangles generated
const int NumVertices = 3 * NumTriangles;

vec2 points[NumVertices];
int Index = 0;

void triangle(const vec2& a, const vec2& b, const vec2& c)
{
	points[Index++] = a;
	points[Index++] = b;
	points[Index++] = c;
}


void divide_triangle(const vec2& a, const vec2& b, const vec2& c, int count)
{
	if (count > 0) {
		vec2 v0 = (a + b) / 2.0;
		vec2 v1 = (a + c) / 2.0;
		vec2 v2 = (b + c) / 2.0;
		divide_triangle(a, v0, v1, count - 1);
		divide_triangle(c, v1, v2, count - 1);
		divide_triangle(b, v2, v0, count - 1);
	}
	else {
		triangle(a, b, c);    // draw triangle at the end of recursion
	}
}

void sierpinski(void) {
	vec2 vertices[3] = {
		vec2(-1.0, -1.0), vec2(0.0, 1.0), vec2(1.0, -1.0)
	};

	// Subdivide the original triangle
	divide_triangle(vertices[0], vertices[1], vertices[2],
		NumTimesToSubdivide);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

}


// Draw Koch Snowflake
GLint iter = 3; // default -> 3 iterations
GLfloat len = 0.5;
GLint kpoints;
GLint kInd;
point2* allKPoints;
GLfloat x_prev = -0.7, y_prev = 0.5;
GLint currentAngle = 0;
void drawKoch(int dir, double len, int iter) {

	//double dirRad = dir*M_PI/180;
	//point2* allKPoints = (point2*)malloc(sizeof(point2) * kpoints);
	float x_next = x_prev + len * cos((dir)*M_PI / 180);
	float y_next = y_prev + len * sin((dir)*M_PI / 180);
	currentAngle = currentAngle + dir;

	if (iter == 0) {

		allKPoints[kInd].x = x_prev;
		allKPoints[kInd].y = y_prev;
		kInd++;

		x_prev = x_next;
		y_prev = y_next;

	}
	else {
		iter--;
		len /= 3;
		drawKoch(dir, len, iter);
		dir += 60;
		drawKoch(dir, len, iter);
		dir -= 120;
		drawKoch(dir, len, iter);
		dir += 60;
		drawKoch(dir, len, iter);

	}
}

void initKoch(void) {
	kpoints = 3 * pow(4, iter);
	kInd = 0;
	allKPoints = (point2*)malloc(sizeof(point2) * kpoints);

	drawKoch(0, len, iter);
	drawKoch(-120, len, iter);
	drawKoch(120, len, iter);

	glBufferData(GL_ARRAY_BUFFER, (sizeof(point2) * kpoints), allKPoints, GL_STATIC_DRAW);
}


void displayKoch(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, width, height);
	glDrawArrays(GL_LINE_LOOP, 0, kpoints);
	glFlush();
}

// polyline drawing function
int drawPolylineFile(char *fileName) {
	std::ifstream infile(fileName);
	glClear(GL_COLOR_BUFFER_BIT);
	float w = width / 4;
	float h = height / 4;
	if (infile) {
		string a;
		char buffer[255];
		bool beforeComment = true;
		bool atExtend = true;
		bool atNumPoly = true;
		float left, top, right, bottom;
		int polyNum;
		while (infile) {
			infile.getline(buffer, 255);
			//if (infile) cout << buffer << endl;
			if (beforeComment == true) {
				// until find * at the start of line
				if (buffer[0] == '*') {
					beforeComment = false;
				}
			}
			else if (atExtend == true) {
				// read for extend
				sscanf(buffer, "%f %f %f %f", &left, &top, &right, &bottom);
				//printf("extend = %f %f %f %f \n", left, top, right, bottom);
				atExtend = false;
			}
			else if (atNumPoly == true) {
				sscanf(buffer, "%d", &polyNum);
				//printf("polyNum = %d \n", polyNum);
				atNumPoly = false;
				break;
			}
		}
		for (int i = 0; i < polyNum; i++) {
			int pointNum;
			float x, y;
			infile.getline(buffer, 255);
			sscanf(buffer, "%d", &pointNum);
			if (pointNum == 0) {
				//cout << buffer << endl;
			}
			//printf("pointNum = %d \n", pointNum);
			//point2 allPoints[pointNum]; //not working
			point2* allPoints = (point2*)malloc(sizeof(point2) * pointNum);
			for (int ij = 0; ij < pointNum; ij++) {
				infile.getline(buffer, 255);
				sscanf(buffer, "%f %f", &x, &y);
				allPoints[ij].x = x;
				allPoints[ij].y = y;
				//printf("x = %f , y = %f \n", allPoints[ij].x, allPoints[ij].y);
			}
			glBufferData(GL_ARRAY_BUFFER, (sizeof(point2) * pointNum), allPoints, GL_STATIC_DRAW);
			for (int k = 0; k < 4; k++) {
				for (int m = 0; m < 4; m++) {
					glViewport(k*w, m*h, w, h);
					glDrawArrays(GL_LINE_STRIP, 0, pointNum);
				}
			}

			glFlush();
		}
		infile.close();
	}

	return 0;
}

void displayTriangle(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glFlush();
}


void initGPUBuffers(void)
{
	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
}


void shaderSetup(void)
{
	// Load shaders and use the resulting shader program
	program = InitShader("vshader1.glsl", "fshader1.glsl");
	glUseProgram(program);

	// Initialize the vertex position attribute from the vertex shader
	GLuint loc = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glClearColor(1.0, 1.0, 1.0, 1.0);        // sets white as color used to clear screen
}


void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 033:
		exit(EXIT_SUCCESS);
		break;

	case 's':
		glViewport(width / 4, height / 4, width / 2, height / 2);
		glClear(GL_COLOR_BUFFER_BIT);
		glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
		glFlush();
		break;

	case 'd':
		glClear(GL_COLOR_BUFFER_BIT);
		drawPolylineFile("dragon.dat");
		break;

	case 'u':
		glClear(GL_COLOR_BUFFER_BIT);
		drawPolylineFile("usa.dat");
		break;

	case 'v':
		glClear(GL_COLOR_BUFFER_BIT);
		drawPolylineFile("vinci.dat");
		break;

	case 'i':
		iter += 1;
		//glClear(GL_COLOR_BUFFER_BIT);
		initKoch();
		glutDisplayFunc(displayKoch);
		glutPostRedisplay();
		break;

	case 'r':
		iter -= 1;
		//glClear(GL_COLOR_BUFFER_BIT);
		initKoch();
		glutDisplayFunc(displayKoch);
		glutPostRedisplay();
		break;

	case 'k':
		iter = 3;
		//glClear(GL_COLOR_BUFFER_BIT);
		initKoch();
		glutDisplayFunc(displayKoch);
		glutPostRedisplay();
		break;
	}
}

int
main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Homework1: Nuttaworn Sujumnong");

	glewInit();

	initGPUBuffers();
	shaderSetup();
	glViewport(width / 4, height / 4, width / 2, height / 2);

	sierpinski();
	initKoch();

	glutDisplayFunc(displayKoch);
	//glutReshapeFunc(myReshape);
	glutKeyboardFunc(keyboard);

	glutMainLoop();
	return 0;
}