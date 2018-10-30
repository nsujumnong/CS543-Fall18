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
GLboolean spin = GL_TRUE;
GLboolean refractTog = GL_TRUE;
GLboolean reflectTog = GL_TRUE;

// handle to program
GLuint program;
GLuint texMapLocation;

using namespace std;
bmpread_t bitmap;
GLuint texture1;
GLuint texture2;
GLuint texture3;
GLuint texture4;
GLuint image1;


const int NumVertices = 18; //(6 faces)(2 triangles/face)(3 vertices/triangle)

							// Vertex data arrays
point4 points[NumVertices];
color4 colors[NumVertices];
point4 normals[18];
vec4   normal;
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
	normal = normalize(cross(vertices[b] - vertices[a], vertices[c] - vertices[b]));
	normals[Index] = normal; colors[Index] = vertex_colors[6]; points[Index] = vertices[a]; tex_coords[Index] = vec2(0.0, 0.0); Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[6]; points[Index] = vertices[b]; tex_coords[Index] = vec2(0.0, 1.0); Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[6]; points[Index] = vertices[c]; tex_coords[Index] = vec2(1.0, 1.0); Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[6]; points[Index] = vertices[a]; tex_coords[Index] = vec2(0.0, 0.0); Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[6]; points[Index] = vertices[c]; tex_coords[Index] = vec2(1.0, 1.0); Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[6]; points[Index] = vertices[d]; tex_coords[Index] = vec2(1.0, 0.0); Index++;
}

// generate 12 triangles: 36 vertices and 36 colors
void colorcube()
{
	//quad( 1, 0, 3, 2 );    
	//quad( 6, 5, 1, 2 );

	//quad( 5, 4, 0, 1 );
}


GLuint texID;
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
	glActiveTexture(GL_TEXTURE0);
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

	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

	const char * suffixes[] = { "posx", "negx", "posy", "negy", "posz", "negz" };

	GLuint targets[] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,  
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,  
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,   
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};


	//glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_RGBA8, 512, 512);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	for (int i = 0; i < 6; i++) {
		string texName = string("nv") + "" + suffixes[i] + ".bmp";
		std::cout << texName.c_str() << endl;
		if (!bmpread(texName.c_str(), 0, &bitmap))
		{
			fprintf(stderr, "%s:error loading bitmap file\n", texName.c_str());
			exit(1);
		}
		bmpread(texName.c_str(), 0, &bitmap);
		glTexImage2D(targets[i], 0, GL_RGB, bitmap.width, bitmap.height, 0, 
			GL_RGB, GL_UNSIGNED_BYTE, bitmap.rgb_data);
	}


	bmpread_free(&bitmap);
	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// sets the default color to clear screen
	glClearColor(1.0, 1.0, 1.0, 1.0); // white background
}
Angel::mat4 modelMat;
GLuint modelMatrix;
void drawWall(void)
{
	// change to GL_FILL
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// draw functions should enable then disable the features 
	// that are specifit the themselves
	// the depth is disabled after the draw 
	// in case you need to draw overlays
	
	glEnable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	modelMat = LookAt(vec4(-0.7, 0.5, 0.5, 1.0), vec4(0.0, 0.0, 0.0, 1.0), vec4(0.0, 1.0, 0.0, 1.0));
	glUniformMatrix4fv(modelMatrix, 1, GL_TRUE, modelMat);
	glUniform1i(glGetUniformLocation(program, "usetexture"), 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glUniform1i(glGetUniformLocation(program, "usetexture"), 1);
	glDrawArrays(GL_TRIANGLES, 6, 12);
	glDisable(GL_DEPTH_TEST);
}

//----------------------------------------------------------------------------
int elemVert;
int elemFace;
vec4* polyVert;
vec4* polyColors;
vec4* polyNormals;
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
		polyNormals = (vec4*)malloc(sizeof(vec4)*numPolyVert);
		for (int j = 0; j < elemFace; j++) {
			file.getline(gchar, 255);
			int num, p, q, r;
			sscanf(gchar, "%d %d %d %d", &num, &p, &q, &r);
			normal = normalize(cross(allVert[q] - allVert[p], allVert[r] - allVert[q]));
			polyNormals[polyIndex] = normal; polyColors[polyIndex] = vertex_colors[1]; polyVert[polyIndex] = vec4(allVert[p].x, allVert[p].y, allVert[p].z, allVert[p].w); polyIndex++;
			polyNormals[polyIndex] = normal; polyColors[polyIndex] = vertex_colors[1]; polyVert[polyIndex] = vec4(allVert[q].x, allVert[q].y, allVert[q].z, allVert[q].w); polyIndex++;
			polyNormals[polyIndex] = normal; polyColors[polyIndex] = vertex_colors[1]; polyVert[polyIndex] = vec4(allVert[r].x, allVert[r].y, allVert[r].z, allVert[r].w); polyIndex++;
		}
	}
}
int numPolyVert;
void drawPolygon(void) {
	numPolyVert = elemFace * 3;
	int allVertices = sizeof(points) + sizeof(vec4)*numPolyVert;
	// Create a vertex array object
	GLuint vao1;
	glGenVertexArrays(1, &vao1);
	glBindVertexArray(vao1);

	GLintptr offset;
	GLuint buffer1;
	glGenBuffers(1, &buffer1);
	glBindBuffer(GL_ARRAY_BUFFER, buffer1);

	// Create and initialize a buffer object
	offset = 0;
	glBufferData(GL_ARRAY_BUFFER, allVertices*3 + sizeof(tex_coords),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points); 	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), (sizeof(vec4)*numPolyVert), polyVert);
	offset += sizeof(points) + (sizeof(vec4)*numPolyVert);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(colors), colors); glBufferSubData(GL_ARRAY_BUFFER, offset+sizeof(colors), (sizeof(vec4)*numPolyVert), polyColors);
	offset += sizeof(colors) + (sizeof(vec4)*numPolyVert);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(normals), normals); glBufferSubData(GL_ARRAY_BUFFER, offset + sizeof(normals), (sizeof(vec4)*numPolyVert), polyNormals);
	offset += sizeof(normals) + (sizeof(vec4)*numPolyVert);
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
	offset += sizeof(points) + (sizeof(vec4)*numPolyVert);

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(offset));
	offset += sizeof(colors) + (sizeof(vec4)*numPolyVert);

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 2, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(offset));
	offset += sizeof(normals) + (sizeof(vec4)*numPolyVert);

	GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(offset));

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);
}

Angel::mat4 ctm = Angel::identity();
Angel::mat4 rY = Angel::RotateY(0.5f);
int usetexSwitch = 2;
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the window

	//Angel::mat4 perspectiveMat = Angel::Perspective((GLfloat)45.0, (GLfloat)width / (GLfloat)height, (GLfloat)0.1, (GLfloat) 100.0);
	Angel::mat4 perspectiveMat = Angel::Ortho(-2.0,2,-2,2,-5,5);
	perspectiveMat = perspectiveMat;

	modelMat = Angel::identity();
	//modelMat = modelMat * Angel::Translate(0.0, 0.0, -1.0f) * Angel::RotateY(45.0f); // *Angel::RotateX(35.0f);
	// set up projection matricies
	modelMatrix = glGetUniformLocationARB(program, "model_matrix");
	glUniformMatrix4fv(modelMatrix, 1, GL_TRUE, modelMat);
	GLuint viewMatrix = glGetUniformLocationARB(program, "projection_matrix");
	glUniformMatrix4fv(viewMatrix, 1, GL_TRUE, perspectiveMat);
	modelMat = LookAt(vec4(-0.7, 0.5, 0.5, 1.0), vec4(0.0, 0.0, 0.0, 1.0), vec4(0.0, 1.0, 0.0, 1.0));
	glUniformMatrix4fv(modelMatrix, 1, GL_TRUE, modelMat);
	drawWall();
	modelMat = modelMat*Scale(0.2,0.2,0.2) ;
	modelMat = modelMat * ctm;
	glActiveTexture(GL_TEXTURE1);
	glUniformMatrix4fv(modelMatrix, 1, GL_TRUE, modelMat);
	if (usetexSwitch == 3) {
		glUniform1i(glGetUniformLocation(program, "usetexture"), 3);
	}
	else if (usetexSwitch == 2) {
		glUniform1i(glGetUniformLocation(program, "usetexture"), 2);
	}
	texMapLocation = glGetUniformLocation(program, "texMap");
	glUniform1i(texMapLocation, 1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
	glDrawArrays(GL_TRIANGLES, 18, numPolyVert); // draw cow
	//glUniform1i(glGetUniformLocation(program, "usetexture"), 0);

	//glFlush(); // force output to graphics hardware

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

void spinPoly() {
	ctm = ctm * rY;
	//cout << ctm << endl;
	glutDisplayFunc(display);
	glutPostRedisplay();

}
void stopTransf() {
	ctm = ctm * Angel::identity();
	glutDisplayFunc(display);
	glutPostRedisplay();
}

void spinRound() {
	if (spin) {
		spinPoly();
	}
	else {
		stopTransf();
	}
}

void refractSwitch() {
	if (refractTog) {
		usetexSwitch = 3;
	}
	else {
		usetexSwitch = 4;
	}
}

void reflectSwitch() {
	if (reflectTog) {
		usetexSwitch = 2;
	}
	else {
		usetexSwitch = 4;
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

	case 'C':
		reflectTog = !reflectTog;
		reflectSwitch();
		glutDisplayFunc(display);
		glutPostRedisplay();
		break;
	case 'D':
		refractTog = !refractTog;
		refractSwitch();
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
	drawPolygon();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(spinRound);
	// should add menus
	// add mouse handler
	// add resize window functionality (should probably try to preserve aspect ratio)

	// enter the drawing loop
	// frame rate can be controlled with 
	glutMainLoop();
	return 0;
}
