// Draws colored cube  

#include "Angel.h"
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
#include "mat.h"
#include "bmpread.h"
//----------------------------------------------------------------------------
class matrix_stack {
public:
	static const int MAX = 50;
	matrix_stack() { index = 0; }
	void push(const mat4& matrix);
	mat4 pop();
private:
	mat4 matrices[MAX];
	int index;
};

void matrix_stack::push(const mat4& matrix) {
	matrices[index] = matrix;
	index++;
}

mat4 matrix_stack::pop() {
	index--;
	return matrices[index];
}


int width = 10;
int height = 10;

// remember to prototype
void generateGeometry(void);
void display(void);
void keyboard(unsigned char key, int x, int y);
void quad(int a, int b, int c, int d);
void colorcube(void);
void drawCube(void);

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

GLboolean texSwitch = GL_TRUE;

// handle to program
GLuint program;

using namespace std;
bmpread_t bitmap;
GLuint texture1;
GLuint texture2;
GLuint texture3;
GLuint texture4;
GLuint image1;
GLuint image2;
GLuint image3;
GLuint image4;
GLuint image5;
GLuint image6;

const int NumVertices = 18; //(6 faces)(2 triangles/face)(3 vertices/triangle)

							// Vertex data arrays
point4 points[NumVertices];
color4 colors[NumVertices];
vec2   tex_coords[NumVertices];

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
	point4(-0.5, -0.5,  0.5, 1.0),
	point4(-0.5,  0.5,  0.5, 1.0),
	point4(0.5,  0.5,  0.5, 1.0),
	point4(0.5, -0.5,  0.5, 1.0),
	point4(-0.5, -0.5, -0.5, 1.0),
	point4(-0.5,  0.5, -0.5, 1.0),
	point4(0.5,  0.5, -0.5, 1.0),
	point4(0.5, -0.5, -0.5, 1.0)
};
// RGBA olors
color4 vertex_colors[8] = {
	color4(0.0, 0.0, 0.0, 1.0),  // black
	color4(1.0, 0.0, 0.0, 1.0),  // red
	color4(1.0, 1.0, 0.0, 1.0),  // yellow
	color4(0.0, 1.0, 0.0, 1.0),  // green
	color4(0.0, 0.0, 1.0, 1.0),  // blue
	color4(1.0, 0.0, 1.0, 1.0),  // magenta
	color4(1.0, 1.0, 1.0, 1.0),  // white
	color4(0.0, 1.0, 1.0, 1.0)   // cyan
};
// quad generates two triangles for each face and assigns colors
//    to the vertices
int Index = 0;
void quad(int a, int b, int c, int d)
{
	colors[Index] = vertex_colors[6]; points[Index] = vertices[a]; tex_coords[Index] = vec2(0.0, 0.0); Index++;
	colors[Index] = vertex_colors[6]; points[Index] = vertices[b]; tex_coords[Index] = vec2(0.0, 1.0); Index++;
	colors[Index] = vertex_colors[6]; points[Index] = vertices[c]; tex_coords[Index] = vec2(1.0, 1.0); Index++;
	colors[Index] = vertex_colors[6]; points[Index] = vertices[a]; tex_coords[Index] = vec2(0.0, 0.0); Index++;
	colors[Index] = vertex_colors[6]; points[Index] = vertices[c]; tex_coords[Index] = vec2(1.0, 1.0); Index++;
	colors[Index] = vertex_colors[6]; points[Index] = vertices[d]; tex_coords[Index] = vec2(1.0, 0.0); Index++;
}

// generate 12 triangles: 36 vertices and 36 colors
void colorcube()
{
	//quad( 1, 0, 3, 2 );    
	//quad( 6, 5, 1, 2 );

	//quad( 5, 4, 0, 1 );
}



void generateGeometry(void)
{

	// Render grass floor
	quad(3, 0, 4, 7); // floor
	quad(2, 3, 7, 6); // right wall
	quad(4, 5, 6, 7); // left wall



	// Load the texture file
	glEnable(GL_TEXTURE_CUBE_MAP);
	if (!bmpread("grass.bmp", 0, &bitmap))
	{
		fprintf(stderr, "%s:error loading bitmap file\n", "grass.bmp");
		exit(1);
	}

	// Initialize texture objects
	glGenTextures(1, &texture1);

	glBindTexture(GL_TEXTURE_2D, texture1);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmap.width, bitmap.height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, bitmap.rgb_data);

	// Render stone walls
	if (!bmpread("stones.bmp", 0, &bitmap))
	{
		fprintf(stderr, "%s:error loading bitmap file\n", "stones.bmp");
		exit(1);
	}

	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmap.width, bitmap.height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, bitmap.rgb_data);

	// Cube map environment mapping


	if (!bmpread("nvnegx.bmp", 0, &bitmap))
	{
		fprintf(stderr, "%s:error loading bitmap file\n", "nvnegx.bmp");
		exit(1);
	}

	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, bitmap.width, bitmap.height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, bitmap.rgb_data);

	if (!bmpread("nvnegy.bmp", 0, &bitmap))
	{
		fprintf(stderr, "%s:error loading bitmap file\n", "nvnegy.bmp");
		exit(1);
	}

	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, bitmap.width, bitmap.height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, bitmap.rgb_data);

	if (!bmpread("nvnegz.bmp", 0, &bitmap))
	{
		fprintf(stderr, "%s:error loading bitmap file\n", "nvnegz.bmp");
		exit(1);
	}

	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, bitmap.width, bitmap.height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, bitmap.rgb_data);

	if (!bmpread("nvposx.bmp", 0, &bitmap))
	{
		fprintf(stderr, "%s:error loading bitmap file\n", "nvposx.bmp");
		exit(1);
	}

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, bitmap.width, bitmap.height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, bitmap.rgb_data);

	if (!bmpread("nvposy.bmp", 0, &bitmap))
	{
		fprintf(stderr, "%s:error loading bitmap file\n", "nvposy.bmp");
		exit(1);
	}

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, bitmap.width, bitmap.height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, bitmap.rgb_data);

	if (!bmpread("nvposz.bmp", 0, &bitmap))
	{
		fprintf(stderr, "%s:error loading bitmap file\n", "nvposz.bmp");
		exit(1);
	}

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, bitmap.width, bitmap.height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, bitmap.rgb_data);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	bmpread_free(&bitmap);
	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(tex_coords),
		NULL, GL_STATIC_DRAW);

	GLintptr offset = 0;
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(points), points);
	offset += sizeof(points);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(colors), colors);
	offset += sizeof(colors);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(tex_coords), tex_coords);

	// Load shaders and use the resulting shader program
	program = InitShader("vshader1.glsl", "fshader1.glsl");
	glUseProgram(program);
	// set up vertex arrays
	offset = 0;
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(offset));
	offset += sizeof(points);

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(offset));
	offset += sizeof(colors);

	GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(offset));

	//glUniform1i(glGetUniformLocation(program, "texture0"), 0);
	//glUniform1i(glGetUniformLocation(program, "texture1"), 1);

	// sets the default color to clear screen
	glClearColor(1.0, 1.0, 1.0, 1.0); // white background
}

void drawWall(void)
{
	// change to GL_FILL
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// draw functions should enable then disable the features 
	// that are specifit the themselves
	// the depth is disabled after the draw 
	// in case you need to draw overlays
	glEnable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glDrawArrays(GL_TRIANGLES, 6, 12);
	glDisable(GL_DEPTH_TEST);
}

//----------------------------------------------------------------------------
int elemVert;
int elemFace;
vec4* polyVert;
vec4* polyColors;
void readPLY() {
	ifstream file("./ply_files/cow.ply");
	string line;
	string elVert;
	string elFace;
	string chkSkip;
	string asc;
	char gchar[255];
	bool isPly = false;
	bool atElVert = true;
	bool atElFace = true;
	bool skipAsc = false;
	bool skipPropXyz = false;
	bool skipProp = false;
	bool endFile = false;
	if (file) {
		while (file) {
			//printf("file is here");
			getline(file, line);
			//file.getline(gchar, 255);
			if (line.find("ply") != string::npos) {
				//if (line == "ply"){
				//if (gchar[0] == 'p' && gchar[1] == 'l' && gchar[2] == 'y') {
				//cout << "ply file" << endl;

			}
			else if (atElVert == true) {
				if (line.find("vertex") != string::npos) {
					//if (gchar[8] == 'v' && gchar[9] == 'e' && gchar[10] == 'r'){
					stringstream ss1(line);
					string tmp1;
					string tmp2;
					ss1 >> tmp1 >> tmp2 >> elemVert;
					cout << "vertices:" << elemVert << endl;
					atElVert = false;
				}
			}
			else if (atElFace == true) {
				if (line.find("face") != string::npos) {
					//if (gchar[8] == 'f' && gchar[9] == 'a' && gchar[10] == 'c') {
					stringstream ss2(line);
					string tmp1;
					string tmp2;
					ss2 >> tmp1 >> tmp2 >> elemFace;
					cout << "faces:" << elemFace << endl;
					atElFace = false;
				}
			}
			else if (endFile == false) {
				if (line.find("end_header") != string::npos) {
					//printf("end_header reached\n");
					endFile = true;
					break;
				}
			}
		}

		vec4* allVert = (vec4*)malloc(sizeof(vec4)*elemVert);
		for (int i = 0; i < elemVert; i++) {
			float x, y, z;
			file.getline(gchar, 255);
			sscanf(gchar, "%f %f %f", &x, &y, &z);
			allVert[i].x = x;
			allVert[i].y = y;
			allVert[i].z = z;
			allVert[i].w = 1.0;

		}
		int polyIndex = 0;
		int numPolyVert = elemFace * 3;

		polyVert = (vec4*)malloc(sizeof(vec4)*numPolyVert);
		polyColors = (vec4*)malloc(sizeof(vec4)*numPolyVert);
		for (int j = 0; j < elemFace; j++) {
			file.getline(gchar, 255);
			int num, p, q, r;
			sscanf(gchar, "%d %d %d %d", &num, &p, &q, &r);
			polyColors[polyIndex] = vertex_colors[1]; polyVert[polyIndex] = vec4(allVert[p].x, allVert[p].y, allVert[p].z, allVert[p].w); polyIndex++;
			polyColors[polyIndex] = vertex_colors[1]; polyVert[polyIndex] = vec4(allVert[q].x, allVert[q].y, allVert[q].z, allVert[q].w); polyIndex++;
			polyColors[polyIndex] = vertex_colors[1]; polyVert[polyIndex] = vec4(allVert[r].x, allVert[r].y, allVert[r].z, allVert[r].w); polyIndex++;

			//glBufferData(GL_ARRAY_BUFFER, (sizeof(vec4)*numPolyVert) + (sizeof(vec4)*numPolyVert),
			//	NULL, GL_STATIC_DRAW);
			//glBufferSubData(GL_ARRAY_BUFFER, 0, (sizeof(vec4)*numPolyVert), polyVert);
			//glBufferSubData(GL_ARRAY_BUFFER, (sizeof(vec4)*numPolyVert), (sizeof(vec4)*numPolyVert), polyColors);
		}
	}
}

void drawPolygon(void) {
	int numPolyVert = elemFace * 3;
	glBufferData(GL_ARRAY_BUFFER, (sizeof(vec4)*numPolyVert) + (sizeof(vec4)*numPolyVert),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, (sizeof(vec4)*numPolyVert), polyVert);
	glBufferSubData(GL_ARRAY_BUFFER, (sizeof(vec4)*numPolyVert), (sizeof(vec4)*numPolyVert), polyColors);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, numPolyVert);
	glDisable(GL_DEPTH_TEST);
}

Angel::mat4 modelMat;
matrix_stack mvstack;
GLuint modelMatrix;

void cow() {
	mvstack.push(modelMat);
	mat4 instance = Translate(0.0, -0.2f, -5.0f)*Scale(0.5f, 0.5f, 0.5f);
	glUniformMatrix4fv(modelMatrix, 1, GL_TRUE, modelMat*instance);
	drawPolygon();
	modelMat = mvstack.pop();
}

Angel::mat4 ctm = Angel::identity();
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the window

	Angel::mat4 perspectiveMat = Angel::Perspective((GLfloat)45.0, (GLfloat)width / (GLfloat)height, (GLfloat)0.1, (GLfloat) 100.0);
	perspectiveMat = perspectiveMat * Translate(0.0, 0.2, -0.2f) * RotateY(45.0f);

	modelMat = Angel::identity();
	//modelMat = modelMat * Angel::Translate(0.0, 0.0, -1.0f) * Angel::RotateY(45.0f); // *Angel::RotateX(35.0f);
	// set up projection matricies
	modelMatrix = glGetUniformLocationARB(program, "model_matrix");
	glUniformMatrix4fv(modelMatrix, 1, GL_TRUE, modelMat);
	GLuint viewMatrix = glGetUniformLocationARB(program, "projection_matrix");
	glUniformMatrix4fv(viewMatrix, 1, GL_TRUE, perspectiveMat);
	
	drawWall();
	
	drawPolygon();

	glFlush(); // force output to graphics hardware

			   // use this call to double buffer
	glutSwapBuffers();
	// you can implement your own buffers with textures
}

//----------------------------------------------------------------------------

void textureSwitch() {
	if (texSwitch) {
		if (!bmpread("grass.bmp", 0, &bitmap))
		{
			fprintf(stderr, "%s:error loading bitmap file\n", "grass.bmp");
			exit(1);
		}
		glGenTextures(1, &texture1);

		glBindTexture(GL_TEXTURE_2D, texture1);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmap.width, bitmap.height, 0,
			GL_RGB, GL_UNSIGNED_BYTE, bitmap.rgb_data);

		if (!bmpread("stones.bmp", 0, &bitmap))
		{
			fprintf(stderr, "%s:error loading bitmap file\n", "stones.bmp");
			exit(1);
		}
		glGenTextures(1, &texture2);

		glBindTexture(GL_TEXTURE_2D, texture2);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmap.width, bitmap.height, 0,
			GL_RGB, GL_UNSIGNED_BYTE, bitmap.rgb_data);
	}
	else {
		if (!bmpread("grey.bmp", 0, &bitmap))
		{
			fprintf(stderr, "%s:error loading bitmap file\n", "grey.bmp");
			exit(1);
		}
		glGenTextures(1, &texture1);

		glBindTexture(GL_TEXTURE_2D, texture1);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmap.width, bitmap.height, 0,
			GL_RGB, GL_UNSIGNED_BYTE, bitmap.rgb_data);

		if (!bmpread("blue.bmp", 0, &bitmap))
		{
			fprintf(stderr, "%s:error loading bitmap file\n", "blue.bmp");
			exit(1);
		}
		glGenTextures(1, &texture2);

		glBindTexture(GL_TEXTURE_2D, texture2);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmap.width, bitmap.height, 0,
			GL_RGB, GL_UNSIGNED_BYTE, bitmap.rgb_data);
	}

}

// keyboard handler
void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 033:
		exit(EXIT_SUCCESS);
		break;

	case 'B':

		texSwitch = !texSwitch;
		textureSwitch();

		glutDisplayFunc(display);
		glutPostRedisplay();
		break;

	}
}

//----------------------------------------------------------------------------
// entry point
int main(int argc, char **argv)
{
	// init glut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	width = 512;
	height = 512;

	// create window
	// opengl can be incorperated into other packages like wxwidgets, fltoolkit, etc.
	glutCreateWindow("Color Cube");

	// init glew
	glewInit();

	generateGeometry();
	readPLY();
	// assign handlers
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	// should add menus
	// add mouse handler
	// add resize window functionality (should probably try to preserve aspect ratio)

	// enter the drawing loop
	// frame rate can be controlled with 
	glutMainLoop();
	return 0;
}
