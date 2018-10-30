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
int width = 0;
int height = 0;

// remember to prototype
void generateGeometry( void );
void display( void );
void keyboard( unsigned char key, int x, int y );
void quad( int a, int b, int c, int d );
void colorcube(void);
void drawCube(void);
void readPLY();
void drawPolygon(void);

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;
typedef Angel::vec3  point3;

// handle to program
GLuint program;
bmpread_t bitmap;
bmpread_t bitmap2;
GLuint textures[2];

typedef vec2 point2;

using namespace std;

const int NumVertices = 18; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 points[NumVertices];
color4 colors[NumVertices];
vec2 tex_coord[NumVertices];
// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
    point4( -0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5, -0.5, -0.5, 1.0 ),
    point4( -0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5, -0.5, -0.5, 1.0 )
};
// RGBA olors
color4 vertex_colors[8] = {
    color4( 0.0, 0.0, 0.0, 1.0 ),  // black
    color4( 1.0, 0.0, 0.0, 1.0 ),  // red
    color4( 1.0, 1.0, 0.0, 1.0 ),  // yellow
    color4( 0.0, 1.0, 0.0, 1.0 ),  // green
    color4( 0.0, 0.0, 1.0, 1.0 ),  // blue
    color4( 1.0, 0.0, 1.0, 1.0 ),  // magenta
    color4( 1.0, 1.0, 1.0, 1.0 ),  // white
    color4( 0.0, 1.0, 1.0, 1.0 )   // cyan
};
// quad generates two triangles for each face and assigns colors
//    to the vertices
int Index = 0;
void quad(int a, int b, int c, int d)
{
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; tex_coord[Index] = point2(0.0, 0.0); Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; tex_coord[Index] = point2(0.0, 1.0); Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; tex_coord[Index] = point2(1.0, 1.0); Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; tex_coord[Index] = point2(0.0, 0.0); Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; tex_coord[Index] = point2(1.0, 1.0); Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; tex_coord[Index] = point2(1.0, 0.0); Index++;
}

// generate 12 triangles: 36 vertices and 36 colors
void colorcube()
{
    //quad( 1, 0, 3, 2 );
    quad( 2, 3, 7, 6 );
    quad( 3, 0, 4, 7 );
    //quad( 6, 5, 1, 2 );
    quad( 4, 5, 6, 7 );
    //quad( 5, 4, 0, 1 );
}

void generateGeometry( void )
{	
    //colorcube();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_CUBE_MAP);
	if (!bmpread("grass.bmp", 0, &bitmap))
	{
		fprintf(stderr, "%s:error loading bitmap file\n", "grass.bmp");
		exit(1);
	}

	if (!bmpread("stones.bmp", 0, &bitmap2))
	{
		fprintf(stderr, "%s:error loading bitmap file\n", "stones.bmp");
		exit(1);
	}

	glActiveTexture(GL_TEXTURE0);
	GLuint texGrass;
	GLuint texStones;

	//GLuint my_tex[2]
	//glGenTexures(2, my_tex)
	glGenTextures(2, textures);

	glBindTexture(GL_TEXTURE_2D, texGrass);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmap.width, bitmap.height, 0,
				 GL_RGB, GL_UNSIGNED_BYTE, bitmap.rgb_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	bmpread_free(&bitmap);

	glBindTexture(GL_TEXTURE_2D, texStones);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmap2.width, bitmap2.height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, bitmap2.rgb_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	bmpread_free(&bitmap2);

	// Set the Tex1 sampler uniform to refer to texture unit 0
	int loc = glGetUniformLocation(program, "Tex1");
	if (loc >= 0) {
		glUniform1i(loc, 0);
	}


	GLuint loc3;

	loc3 = glGetAttribLocation(program, "texcoord");
	glEnableVertexAttribArray(loc3);
	glVertexAttribPointer(loc3, 2, GL_FLOAT, GL_FALSE, 0, tex_coord);

    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object
	
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors),
		  NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors );


	// Load shaders and use the resulting shader program
    program = InitShader( "vshader1.glsl", "fshader1.glsl" );
    glUseProgram( program );
     // set up vertex arrays
	GLintptr offset;
	offset = 0;
	GLsizeiptr size = sizeof(points) + sizeof(colors) + sizeof(tex_coord);

    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(offset) );

	offset += sizeof(points);
    GLuint vColor = glGetAttribLocation( program, "vColor" ); 
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(offset) );

	offset += sizeof(colors);
	GLuint vTexCoord = glGetAttribLocation(program, "vTexCoords"); 
	glEnableVertexAttribArray(vTexCoord); 
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, 
			   BUFFER_OFFSET(offset));

	// Set the value of the fragment shader texture sampler variable 
	//   ("texture") to the appropriate texture unit. 
	glUniform1i(glGetUniformLocation(program, "Tex1"), 0);

	// sets the default color to clear screen
    glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background
}

void drawCube(void)
{
	// change to GL_FILL
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	// draw functions should enable then disable the features 
	// that are specifit the themselves
	// the depth is disabled after the draw 
	// in case you need to draw overlays
	glEnable( GL_DEPTH_TEST );
    glDrawArrays( GL_TRIANGLES, 0, NumVertices );
	glDisable( GL_DEPTH_TEST ); 
}



//----------------------------------------------------------------------------
// this is where the drawing should happen
void display( void )
{
	// SOME RANDOM TIPS
	//========================================================================
	// remember to enable depth buffering when drawing in 3d

	// avoid using glTranslatex, glRotatex, push and pop
	// pass your own view matrix to the shader directly
	// refer to the latest OpenGL documentation for implementation details

    // Do not set the near and far plane too far appart!
	// depth buffers do not have unlimited resolution
	// surfaces will start to fight as they come nearer to each other
	// if the planes are too far appart (quantization errors :(   )

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );     // clear the window

	Angel::mat4 perspectiveMat = Angel::Perspective((GLfloat)45.0, (GLfloat)width/(GLfloat)height, (GLfloat)0.1, (GLfloat) 100.0);

	float viewMatrixf[16];
	viewMatrixf[0] = perspectiveMat[0][0];viewMatrixf[4] = perspectiveMat[0][1];
	viewMatrixf[1] = perspectiveMat[1][0];viewMatrixf[5] = perspectiveMat[1][1];
	viewMatrixf[2] = perspectiveMat[2][0];viewMatrixf[6] = perspectiveMat[2][1];
	viewMatrixf[3] = perspectiveMat[3][0];viewMatrixf[7] = perspectiveMat[3][1];

	viewMatrixf[8] = perspectiveMat[0][2];viewMatrixf[12] = perspectiveMat[0][3];
	viewMatrixf[9] = perspectiveMat[1][2];viewMatrixf[13] = perspectiveMat[1][3];
	viewMatrixf[10] = perspectiveMat[2][2];viewMatrixf[14] = perspectiveMat[2][3];
	viewMatrixf[11] = perspectiveMat[3][2];viewMatrixf[15] = perspectiveMat[3][3];
	
	Angel::mat4 modelMat = Angel::identity();
	modelMat = modelMat * Angel::Translate(0.0, 0.0, -2.0f) * Angel::RotateY(45.0f) * Angel::RotateX(35.0f);
	float modelMatrixf[16];
	modelMatrixf[0] = modelMat[0][0];modelMatrixf[4] = modelMat[0][1];
	modelMatrixf[1] = modelMat[1][0];modelMatrixf[5] = modelMat[1][1];
	modelMatrixf[2] = modelMat[2][0];modelMatrixf[6] = modelMat[2][1];
	modelMatrixf[3] = modelMat[3][0];modelMatrixf[7] = modelMat[3][1];

	modelMatrixf[8] = modelMat[0][2];modelMatrixf[12] = modelMat[0][3];
	modelMatrixf[9] = modelMat[1][2];modelMatrixf[13] = modelMat[1][3];
	modelMatrixf[10] = modelMat[2][2];modelMatrixf[14] = modelMat[2][3];
	modelMatrixf[11] = modelMat[3][2];modelMatrixf[15] = modelMat[3][3];
	
	// set up projection matricies
	GLuint modelMatrix = glGetUniformLocationARB(program, "model_matrix");
	glUniformMatrix4fv( modelMatrix, 1, GL_FALSE, modelMatrixf );
	GLuint viewMatrix = glGetUniformLocationARB(program, "projection_matrix");
	glUniformMatrix4fv( viewMatrix, 1, GL_FALSE, viewMatrixf);

	drawCube();
    glFlush(); // force output to graphics hardware

	// use this call to double buffer
	glutSwapBuffers();
	// you can implement your own buffers with textures
}
//----------------------------------------------------------------------------
// list of PLY files
//enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
// ---------------------------------------------------------------------------
GLfloat theta = 0.0;
GLfloat radius = 1.0;
GLfloat phi = 0.0;
// Draw mesh
int elemVert;
int elemFace;
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
	//int elemVert;
	//int elemFace;
	if(file){
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
		int randColor = rand() % 8;
		vec4* polyVert = (vec4*)malloc(sizeof(vec4)*numPolyVert);
		vec4* polyColors = (vec4*)malloc(sizeof(vec4)*numPolyVert);
		for (int j = 0; j < elemFace; j++) {
			file.getline(gchar, 255);
			int num, p, q, r;
			sscanf(gchar, "%d %d %d %d", &num, &p, &q, &r);
			polyColors[polyIndex] = vertex_colors[randColor]; polyVert[polyIndex] = vec4(allVert[p].x, allVert[p].y, allVert[p].z, allVert[p].w); polyIndex++;
			polyColors[polyIndex] = vertex_colors[randColor]; polyVert[polyIndex] = vec4(allVert[q].x, allVert[q].y, allVert[q].z, allVert[q].w); polyIndex++;
			polyColors[polyIndex] = vertex_colors[randColor]; polyVert[polyIndex] = vec4(allVert[r].x, allVert[r].y, allVert[r].z, allVert[r].w); polyIndex++;
		}

		/*for (int k = 0; k < numPolyVert; k++) {
			cout << "vertex no." << k << ": " << polyVert[k] << endl;
		}*/

		glBufferData(GL_ARRAY_BUFFER, (sizeof(vec4)*numPolyVert) + (sizeof(vec4)*numPolyVert),
			NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, (sizeof(vec4)*numPolyVert), polyVert);
		glBufferSubData(GL_ARRAY_BUFFER, (sizeof(vec4)*numPolyVert), (sizeof(vec4)*numPolyVert), polyColors);

	}
}

void drawPolygon(void) {
	int numPolyVert = elemFace * 3;
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, numPolyVert);
	glDisable(GL_DEPTH_TEST);
}
//
GLfloat h = 0.01;
Angel::mat4 ctm = Angel::identity();
Angel::mat4 tXL = Angel::Translate(-1.0f, 0.0, 0.0);
Angel::mat4 tXR = Angel::Translate(1.0f, 0.0, 0.0);
Angel::mat4 tYL = Angel::Translate(0.0, 1.0f, 0.0);
Angel::mat4 tYR = Angel::Translate(0.0, -1.0f, 0.0);
Angel::mat4 tZL = Angel::Translate(0.0, 0.0, 1.0f);
Angel::mat4 tZR = Angel::Translate(0.0, 0.0, -1.0f);
Angel::mat4 rY = Angel::RotateY(0.5f);
Angel::mat4 shearInc = mat4(1.0,   h, 0.0, 0.0,
						    0.0, 1.0, 0.0, 0.0,
							0.0, 0.0, 1.0, 0.0,
							0.0, 0.0, 0.0, 1.0);
Angel::mat4 shearDec = mat4(1.0,  -h, 0.0, 0.0,
							0.0, 1.0, 0.0, 0.0,
							0.0, 0.0, 1.0, 0.0,
							0.0, 0.0, 0.0, 1.0);

Angel::mat4 twistInc = mat4(1.0,   h,   h, 0.0,
							0.0, 1.0, 0.0, 0.0,
							0.0, 0.0, 1.0, 0.0,
							0.0, 0.0, 0.0, 1.0);
Angel::mat4 twistDec = mat4(1.0,  -h,  -h, 0.0,
							0.0, 1.0, 0.0, 0.0,
							0.0, 0.0, 1.0, 0.0,
							0.0, 0.0, 0.0, 1.0);

//GLint matrix_loc;
void displayPoly(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Angel::mat4 perspectiveMat = Angel::Perspective((GLfloat)45.0, (GLfloat)width / (GLfloat)height, (GLfloat)0.1, (GLfloat) 100.0);
	perspectiveMat = perspectiveMat*Angel::Translate(0.0, -2.0f, -10.f);

	float viewMatrixf[16];
	viewMatrixf[0] = perspectiveMat[0][0]; viewMatrixf[4] = perspectiveMat[0][1];
	viewMatrixf[1] = perspectiveMat[1][0]; viewMatrixf[5] = perspectiveMat[1][1];
	viewMatrixf[2] = perspectiveMat[2][0]; viewMatrixf[6] = perspectiveMat[2][1];
	viewMatrixf[3] = perspectiveMat[3][0]; viewMatrixf[7] = perspectiveMat[3][1];

	viewMatrixf[8] = perspectiveMat[0][2]; viewMatrixf[12] = perspectiveMat[0][3];
	viewMatrixf[9] = perspectiveMat[1][2]; viewMatrixf[13] = perspectiveMat[1][3];
	viewMatrixf[10] = perspectiveMat[2][2]; viewMatrixf[14] = perspectiveMat[2][3];
	viewMatrixf[11] = perspectiveMat[3][2]; viewMatrixf[15] = perspectiveMat[3][3];

	Angel::mat4 modelMat = Angel::identity();
	//modelMat = modelMat * Angel::Translate(0.0, 0.0, -2.0f) * Angel::RotateY(45.0f) * Angel::RotateX(35.0f);
	
	modelMat = modelMat * ctm;
	//cout << modelMat << endl;
	float modelMatrixf[16];
	modelMatrixf[0] = modelMat[0][0]; modelMatrixf[4] = modelMat[0][1];
	modelMatrixf[1] = modelMat[1][0]; modelMatrixf[5] = modelMat[1][1];
	modelMatrixf[2] = modelMat[2][0]; modelMatrixf[6] = modelMat[2][1];
	modelMatrixf[3] = modelMat[3][0]; modelMatrixf[7] = modelMat[3][1];

	modelMatrixf[8] = modelMat[0][2]; modelMatrixf[12] = modelMat[0][3];
	modelMatrixf[9] = modelMat[1][2]; modelMatrixf[13] = modelMat[1][3];
	modelMatrixf[10] = modelMat[2][2]; modelMatrixf[14] = modelMat[2][3];
	modelMatrixf[11] = modelMat[3][2]; modelMatrixf[15] = modelMat[3][3];

	// set up projection matricies
	GLuint modelMatrix = glGetUniformLocationARB(program, "model_matrix");
	glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, modelMatrixf);
	GLuint viewMatrix = glGetUniformLocationARB(program, "projection_matrix");
	glUniformMatrix4fv(viewMatrix, 1, GL_FALSE, viewMatrixf);
	
	drawPolygon();
	glFlush();
	glutSwapBuffers();

}
//----------------------------------------------------------------------------
double togR = 0;
double rotSwitch;
int test1 = 0;
int testPow;

void spinPoly() {
	ctm = ctm * rY;
	//cout << ctm << endl;
	glutDisplayFunc(displayPoly);
	glutPostRedisplay();
		
}

void stopSpin() {
	ctm = ctm * Angel::identity();
	glutDisplayFunc(displayPoly);
	glutPostRedisplay();
}
// keyboard handler
void keyboard( unsigned char key, int x, int y )
{
    switch ( key ) {
    case 033:
        exit( EXIT_SUCCESS );
        break;

	case 'X':
		ctm = ctm * tXR;
		glutDisplayFunc(displayPoly);
		glutPostRedisplay();
		break;

	case 'x':
		ctm = ctm * tXL;
		glutDisplayFunc(displayPoly);
		glutPostRedisplay();
		break;

	case 'R':
		/*ctm = ctm * rY;
		glutDisplayFunc(displayPoly);
		glutPostRedisplay();*/
		glutIdleFunc(spinPoly);
		break;

	}

}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-50.0, 50.0, -50.0, 50.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
//----------------------------------------------------------------------------
// entry point
int main( int argc, char **argv )
{
	// init glut
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( 512, 512 );
	width = 512;
	height = 512;

	// create window
	// opengl can be incorperated into other packages like wxwidgets, fltoolkit, etc.
    //glutCreateWindow( "Color Cube" );
	glutCreateWindow("Nuttaworn Sujumnong HW2");

	// init glew
    glewInit();

    generateGeometry();

	// assign handlers
    //glutDisplayFunc( display );
	readPLY();
	glutDisplayFunc(displayPoly);
	glutReshapeFunc(reshape);
    glutKeyboardFunc( keyboard );
	glutIdleFunc(spinPoly);
	// should add menus
	// add mouse handler
	// add resize window functionality (should probably try to preserve aspect ratio)

	// enter the drawing loop
	// frame rate can be controlled with 
    glutMainLoop();
    return 0;
}
