// Draws colored cube  

#include "Angel.h"
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
#include "mat.h"
#include <math.h>

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


int width = 0;
int height = 0;

// remember to prototype
void generateGeometry( void );
void display( void );
void keyboard( unsigned char key, int x, int y );
void quad( int a, int b, int c, int d );
void colorcube(void);
void drawCube(void);
void readPLY(char *filename);
void drawPolygon(void);

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;
typedef Angel::vec3  point3;

GLboolean spin = GL_TRUE;
GLboolean slideXR = GL_FALSE;
GLboolean slideXL = GL_FALSE;
GLboolean slideYR = GL_FALSE;
GLboolean slideYL = GL_FALSE;
GLboolean slideZR = GL_FALSE;
GLboolean slideZL = GL_FALSE;
GLboolean wave = GL_FALSE;
// handle to program
GLuint program;

using namespace std;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 points[NumVertices];
color4 colors[NumVertices];

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
void quad( int a, int b, int c, int d )
{
    colors[Index] = vertex_colors[1]; points[Index] = vertices[a]; Index++;
    colors[Index] = vertex_colors[1]; points[Index] = vertices[b]; Index++;
    colors[Index] = vertex_colors[1]; points[Index] = vertices[c]; Index++;
    colors[Index] = vertex_colors[1]; points[Index] = vertices[a]; Index++;
    colors[Index] = vertex_colors[1]; points[Index] = vertices[c]; Index++;
    colors[Index] = vertex_colors[1]; points[Index] = vertices[d]; Index++;
}

// generate 12 triangles: 36 vertices and 36 colors
void colorcube()
{
    quad( 1, 0, 3, 2 );
    quad( 2, 3, 7, 6 );
    quad( 3, 0, 4, 7 );
    quad( 6, 5, 1, 2 );
    quad( 4, 5, 6, 7 );
    quad( 5, 4, 0, 1 );
}

//void generateGeometry( void )
//{	
//    //colorcube();
//
//    // Create a vertex array object
//    GLuint vao;
//    glGenVertexArrays( 1, &vao );
//    glBindVertexArray( vao );
//
//    // Create and initialize a buffer object
//    GLuint buffer;
//    glGenBuffers( 1, &buffer );
//    glBindBuffer( GL_ARRAY_BUFFER, buffer );
//    /*glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors),
//		  NULL, GL_STATIC_DRAW );
//    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
//    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors );
//	*/
//
//	// Load shaders and use the resulting shader program
//    program = InitShader( "vshader1.glsl", "fshader1.glsl" );
//    glUseProgram( program );
//     // set up vertex arrays
//    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
//    glEnableVertexAttribArray( vPosition );
//    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
//			   BUFFER_OFFSET(0) );
//
//    GLuint vColor = glGetAttribLocation( program, "vColor" ); 
//    glEnableVertexAttribArray( vColor );
//    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,
//			   BUFFER_OFFSET(sizeof(points)) );
//
//	// sets the default color to clear screen
//    glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background
//}

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
int plyInd = 1;
char * plyList[] = {
	"./ply_files/airplane.ply",
	"./ply_files/ant.ply",
	"./ply_files/apple.ply",
	"./ply_files/balance.ply",
	"./ply_files/beethoven.ply",
	"./ply_files/big_atc.ply",
	"./ply_files/big_dodge.ply",
	"./ply_files/big_porsche.ply",
	"./ply_files/big_spider.ply",
	"./ply_files/canstick.ply",
	"./ply_files/chopper.ply",
	"./ply_files/cow.ply",
	"./ply_files/dolphins.ply",
	"./ply_files/egret.ply",
	"./ply_files/f16.ply",
	"./ply_files/footbones.ply",
	"./ply_files/fracttree.ply",
	"./ply_files/galleon.ply",
	"./ply_files/hammerhead.ply",
	"./ply_files/helix.ply",
	"./ply_files/hind.ply",
	"./ply_files/kerolamp.ply",
	"./ply_files/ketchup.ply",
	"./ply_files/mug.ply",
	"./ply_files/part.ply",
	"./ply_files/pickup_big.ply",
	"./ply_files/pump.ply",
	"./ply_files/pumpa_tb.ply",
	"./ply_files/sandal.ply",
	"./ply_files/saratoga.ply",
	"./ply_files/scissors.ply",
	"./ply_files/shark.ply",
	"./ply_files/steeringweel.ply",
	"./ply_files/stratocaster.ply",
	"./ply_files/street_lamp.ply",
	"./ply_files/teapot.ply",
	"./ply_files/tennis_shoe.ply",
	"./ply_files/tommygun.ply",
	"./ply_files/trashcan.ply",
	"./ply_files/turbine.ply",
	"./ply_files/urn2.ply",
	"./ply_files/walkman.ply",
	"./ply_files/weathervane.ply",
};
// ---------------------------------------------------------------------------
GLfloat theta = 0.0;
GLfloat radius = 1.0;
GLfloat phi = 0.0;
// Draw mesh
int elemVert;
int elemFace;
color4 colorRed = color4(1.0, 0.0, 0.0, 1.0);
vec4* polyVert;
vec4* polyColors;
vec4* allVert;
void readPLY(char *filename) {
	ifstream file(filename);
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
	//cout << "The current wire frame is" << filename << endl;
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
					//cout << "vertices:" << elemVert << endl;
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
					//cout << "faces:" << elemFace << endl;
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

		allVert = (vec4*)malloc(sizeof(vec4)*elemVert);
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
		//int randColor = rand() % 8;
		//int randColor = 1;  // get only the color red
		polyVert = (vec4*)malloc(sizeof(vec4)*numPolyVert);
		polyColors = (vec4*)malloc(sizeof(vec4)*numPolyVert);
		for (int j = 0; j < elemFace; j++) {
			file.getline(gchar, 255);
			int num, p, q, r;
			sscanf(gchar, "%d %d %d %d", &num, &p, &q, &r);
			color4 vertexColor = vertex_colors[1];
			polyColors[polyIndex] = color4(1.0, 0.0, 0.0, 1.0); polyVert[polyIndex] = vec4(allVert[p].x, allVert[p].y, allVert[p].z, allVert[p].w); polyIndex++;
			polyColors[polyIndex] = color4(1.0, 0.0, 0.0, 1.0); polyVert[polyIndex] = vec4(allVert[q].x, allVert[q].y, allVert[q].z, allVert[q].w); polyIndex++;
			polyColors[polyIndex] = color4(1.0, 0.0, 0.0, 1.0); polyVert[polyIndex] = vec4(allVert[r].x, allVert[r].y, allVert[r].z, allVert[r].w); polyIndex++;
		}

		glBufferData(GL_ARRAY_BUFFER, (sizeof(vec4)*numPolyVert) + (sizeof(vec4)*numPolyVert),
			NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, (sizeof(vec4)*numPolyVert), polyVert);
		//glBufferSubData(GL_ARRAY_BUFFER, (sizeof(vec4)*numPolyVert), (sizeof(vec4)*numPolyVert), polyColors);

	}
}

void generateGeometry(void)
{
	//colorcube();

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	/*glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors),
	NULL, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors );
	*/

	// Load shaders and use the resulting shader program
	program = InitShader("vshader1.glsl", "fshader1.glsl");
	glUseProgram(program);
	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(polyVert)));

	// Initialize shader lighting parameters
	point4 light_position(0.0, 0.0, 2.0, 0.0);
	color4 light_ambient(0.2, 0.2, 0.2, 1.0);
	color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
	color4 light_specular(1.0, 1.0, 1.0, 1.0);

	color4 material_ambient(1.0, 0.0, 1.0, 1.0);
	color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
	color4 material_specular(1.0, 0.0, 1.0, 1.0);
	float material_shininess = 5.0;

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"),
		1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"),
		1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"),
		1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"),
		1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"),
		material_shininess);
	// sets the default color to clear screen
	glClearColor(1.0, 1.0, 1.0, 1.0); // white background
}
//mat4 model_view;
//matrix_stack mvstack;
void drawPolygon(void) {
	int numPolyVert = elemFace * 3;
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, numPolyVert);
	glDisable(GL_DEPTH_TEST);
}
int elemAnt;
vec4* vertAnt;
vec4* colorAnt;
vec4 *boxAntVert;
GLfloat xmin, xmax, ymin, ymax, zmin, zmax;

void getAnt(void) {
	readPLY("./ply_files/ant.ply");
	elemAnt = elemFace;
	//vec4* allVertAnt = allVert;
	int numPolyVert = elemAnt*3;
	vertAnt = polyVert;
	colorAnt = polyColors;
	xmin = 0;
	xmax = 0;
	ymin = 0;
	ymax = 0;
	zmin = 0;
	zmax = 0;
	for (int i = 0; i < elemVert; i++) {
		if (allVert[i].x < xmin) {
			xmin = allVert[i].x;
			//cout << "minx: " << allVert[i].x << endl;
		}
		else if (allVert[i].x > xmax) {
			xmax = allVert[i].x;
			//cout << "maxx: " << xmax << endl;
		}
	 }
	for (int j = 0; j < elemVert; j++) {
		if (allVert[j].y < ymin) {
			ymin = allVert[j].y;
			//cout << "minx: " << allVert[i].x << endl;
		}
		else if (allVert[j].y > ymax) {
			ymax = allVert[j].y;
			//cout << "maxx: " << xmax << endl;
		}
	}
	for (int k = 0; k < elemVert; k++) {
		if (allVert[k].z < zmin) {
			zmin = allVert[k].z;
			//cout << "minx: " << allVert[i].x << endl;
		}
		else if (allVert[k].z > zmax) {
			zmax = allVert[k].z;
			//cout << "maxx: " << xmax << endl;
		}
	}

	/*boxAntVert[8] = {
		point4(xmin, ymin,  zmax, 1.0),
		point4(xmin, ymax,  zmax, 1.0),
		point4(xmax, ymax,  zmax, 1.0),
		point4(xmax, ymin,  zmax, 1.0),
		point4(xmin, ymin,  zmin, 1.0),
		point4(xmin, ymax,  zmin, 1.0),
		point4(xmax, ymax,	zmin, 1.0),
		point4(xmax, ymin,  zmin, 1.0)
	};
	*/
}

void drawAnt(void) {
	int numPolyVert = elemAnt * 3;
	glBufferData(GL_ARRAY_BUFFER, (sizeof(vec4)*numPolyVert) + (sizeof(vec4)*numPolyVert),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, (sizeof(vec4)*numPolyVert), vertAnt);
	glBufferSubData(GL_ARRAY_BUFFER, (sizeof(vec4)*numPolyVert), (sizeof(vec4)*numPolyVert), colorAnt);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, numPolyVert);
	glDisable(GL_DEPTH_TEST);
}

void boxAnt() {

}

int elemApple;
vec4* vertApple;
vec4* colorApple;
void getApple(void) {
	readPLY("./ply_files/cow.ply");
	elemApple = elemFace;
	int numPolyVert = elemApple*3;
	vertApple = polyVert;
	colorApple = polyColors;
}

void drawApple(void) {
	int numPolyVert = elemApple * 3;
	glBufferData(GL_ARRAY_BUFFER, (sizeof(vec4)*numPolyVert) + (sizeof(vec4)*numPolyVert),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, (sizeof(vec4)*numPolyVert), vertApple);
	glBufferSubData(GL_ARRAY_BUFFER, (sizeof(vec4)*numPolyVert), (sizeof(vec4)*numPolyVert), colorApple);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, numPolyVert);
	glDisable(GL_DEPTH_TEST);
}

int elemBeethoven;
vec4* vertBeethoven;
vec4* colorBeethoven;
void getBeethoven(void) {
	readPLY("./ply_files/beethoven.ply");
	elemBeethoven = elemFace;
	int numPolyVert = elemBeethoven * 3;
	vertBeethoven = polyVert;
	colorBeethoven = polyColors;
}
void drawBeethoven(void) {
	int numPolyVert = elemBeethoven * 3;
	glBufferData(GL_ARRAY_BUFFER, (sizeof(vec4)*numPolyVert) + (sizeof(vec4)*numPolyVert),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, (sizeof(vec4)*numPolyVert), vertBeethoven);
	glBufferSubData(GL_ARRAY_BUFFER, (sizeof(vec4)*numPolyVert), (sizeof(vec4)*numPolyVert), colorBeethoven);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, numPolyVert);
	glDisable(GL_DEPTH_TEST);
}

int elemATC;
vec4* vertATC;
vec4* colorATC;
void getATC(void) {
	readPLY("./ply_files/big_atc.ply");
	elemATC = elemFace;
	int numPolyVert = elemATC * 3;
	vertATC = polyVert;
	colorATC = polyColors;
}
void drawATC(void) {
	int numPolyVert = elemATC * 3;
	glBufferData(GL_ARRAY_BUFFER, (sizeof(vec4)*numPolyVert) + (sizeof(vec4)*numPolyVert),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, (sizeof(vec4)*numPolyVert), vertATC);
	glBufferSubData(GL_ARRAY_BUFFER, (sizeof(vec4)*numPolyVert), (sizeof(vec4)*numPolyVert), colorATC);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, numPolyVert);
	glDisable(GL_DEPTH_TEST);
}

int elemDodge;
vec4* vertDodge;
vec4* colorDodge;

void getDodge(void) {
	readPLY("./ply_files/big_dodge.ply");
	elemDodge = elemFace;
	int numPolyVert = elemDodge * 3;
	vertDodge = polyVert;
	colorDodge = polyColors;
}
void drawDodge(void) {
	int numPolyVert = elemDodge * 3;
	glBufferData(GL_ARRAY_BUFFER, (sizeof(vec4)*numPolyVert) + (sizeof(vec4)*numPolyVert),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, (sizeof(vec4)*numPolyVert), vertDodge);
	glBufferSubData(GL_ARRAY_BUFFER, (sizeof(vec4)*numPolyVert), (sizeof(vec4)*numPolyVert), colorDodge);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, numPolyVert);
	glDisable(GL_DEPTH_TEST);
}

int elemPorsche;
vec4* vertPorsche;
vec4* colorPorsche;
void getPorsche(void) {
	readPLY("./ply_files/big_porsche.ply");
	elemPorsche = elemFace;
	int numPolyVert = elemPorsche * 3;
	vertPorsche = polyVert;
	colorPorsche = polyColors;
}
void drawPorsche(void) {
	int numPolyVert = elemPorsche * 3;
	glBufferData(GL_ARRAY_BUFFER, (sizeof(vec4)*numPolyVert) + (sizeof(vec4)*numPolyVert),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, (sizeof(vec4)*numPolyVert), vertPorsche);
	glBufferSubData(GL_ARRAY_BUFFER, (sizeof(vec4)*numPolyVert), (sizeof(vec4)*numPolyVert), colorPorsche);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, numPolyVert);
	glDisable(GL_DEPTH_TEST);
}

GLfloat h = 0.01;
Angel::mat4 ctm = Angel::identity();
Angel::mat4 ctmRev = Angel::identity();
Angel::mat4 tXL = Angel::Translate(-1.0f, 0.0, 0.0);
Angel::mat4 tXR = Angel::Translate(1.0f, 0.0, 0.0);
Angel::mat4 tYL = Angel::Translate(0.0, 1.0f, 0.0);
Angel::mat4 tYR = Angel::Translate(0.0, -1.0f, 0.0);
Angel::mat4 tZL = Angel::Translate(0.0, 0.0, 1.0f);
Angel::mat4 tZR = Angel::Translate(0.0, 0.0, -1.0f);
Angel::mat4 scaleApple = Angel::Scale(10.0f, 10.0f, 10.0f);
Angel::mat4 rY = Angel::RotateY(0.5f);
Angel::mat4 rYRev = Angel::RotateY(-0.5f);
//Angel::mat4 sinZ = Angel::Translate(0.0, );
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

Angel::mat4 modelMat;
matrix_stack mvstack;
GLuint modelMatrix;

void ant() {
	mvstack.push(modelMat);
	mat4 instance = Translate(0.0,1.5f,0.0f)*Scale(0.5f,0.5f,0.5f);
	glUniformMatrix4fv(modelMatrix, 1, GL_TRUE, modelMat*instance);
	drawAnt();
	modelMat = mvstack.pop();
}

void apple() {
	mvstack.push(modelMat);
	mat4 instance = Translate(0.0f,0.0f,0.0)*Scale(1.0f,1.0f,1.0f);
	glUniformMatrix4fv(modelMatrix, 1, GL_TRUE, modelMat*instance);
	drawApple();
	modelMat = mvstack.pop();
}

void beethoven() {
	mvstack.push(modelMat);
	mat4 instance = Translate(0.0f, 0.0f, 0.0)*Scale(0.2f,0.2f,0.2f);
	glUniformMatrix4fv(modelMatrix, 1, GL_TRUE, modelMat*instance);
	drawBeethoven();
	modelMat = mvstack.pop();
}

void bigATC() {
	mvstack.push(modelMat);
	mat4 instance = Translate(0.0f, 0.0f, 0.0)*Scale(1.0f, 1.0f, 1.0f);
	glUniformMatrix4fv(modelMatrix, 1, GL_TRUE, modelMat*instance);
	drawATC();
	modelMat = mvstack.pop();
}

void bigDodge() {
	mvstack.push(modelMat);
	mat4 instance = Translate(0.0f, 0.0f, 0.0)*Scale(1.0f, 1.0f, 1.0f);
	glUniformMatrix4fv(modelMatrix, 1, GL_TRUE, modelMat*instance);
	drawDodge();
	modelMat = mvstack.pop();
}

void bigPorsche() {
	mvstack.push(modelMat);
	mat4 instance = Translate(0.0f, 0.0f, 0.0)*Scale(1.0f, 1.0f, 1.0f);
	glUniformMatrix4fv(modelMatrix, 1, GL_TRUE, modelMat*instance);
	drawPorsche();
	modelMat = mvstack.pop();
}

double thetaSin = 0;
double thetaSinNeg = 0;
double sinVal;
double sinValNeg;
void displayPoly(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Angel::mat4 perspectiveMat = Angel::Perspective((GLfloat)45.0, (GLfloat)width / (GLfloat)height, (GLfloat)0.1, (GLfloat) 100.0);
	perspectiveMat = perspectiveMat * Translate(0.0, 10.0f, -60.0f);

	float viewMatrixf[16];
	viewMatrixf[0] = perspectiveMat[0][0]; viewMatrixf[4] = perspectiveMat[0][1];
	viewMatrixf[1] = perspectiveMat[1][0]; viewMatrixf[5] = perspectiveMat[1][1];
	viewMatrixf[2] = perspectiveMat[2][0]; viewMatrixf[6] = perspectiveMat[2][1];
	viewMatrixf[3] = perspectiveMat[3][0]; viewMatrixf[7] = perspectiveMat[3][1];

	viewMatrixf[8] = perspectiveMat[0][2]; viewMatrixf[12] = perspectiveMat[0][3];
	viewMatrixf[9] = perspectiveMat[1][2]; viewMatrixf[13] = perspectiveMat[1][3];
	viewMatrixf[10] = perspectiveMat[2][2]; viewMatrixf[14] = perspectiveMat[2][3];
	viewMatrixf[11] = perspectiveMat[3][2]; viewMatrixf[15] = perspectiveMat[3][3];

	modelMat = Angel::identity();
	
	//cout << thetaSin << endl;
	//modelMat = modelMat * Angel::Translate(0.0, 0.0, -2.0f) * Angel::RotateY(45.0f) * Angel::RotateX(35.0f);
	//mvstack.push(modelMat);
	modelMat = modelMat * ctm;
	sinVal = sin(thetaSin)*1.5;
	sinValNeg = sin(thetaSinNeg)*1.5;
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
	modelMatrix = glGetUniformLocationARB(program, "model_matrix");
	glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, modelMatrixf);
	GLuint viewMatrix = glGetUniformLocationARB(program, "projection_matrix");
	glUniformMatrix4fv(viewMatrix, 1, GL_FALSE, viewMatrixf);

	//drawPolygon();
	mvstack.push(modelMat);
	ant();

	mvstack.push(modelMat);
	modelMat = modelMat * Translate(-15.0f, -10.0f + sinVal, 0.0f)*ctmRev;
	apple();

	mvstack.push(modelMat);
	modelMat = modelMat * Translate(-5.0f, -5.0f + sinVal*1.5, 0.0f) * Scale(1.5f, 1.5f, 1.5f) * ctm;
	beethoven();
	modelMat = mvstack.pop();

	mvstack.push(modelMat);
	modelMat = modelMat * Translate(5.0f, -5.0f + sinValNeg*1.5, 0.0f) * Scale(0.7f, 0.7f, 0.7f) * ctm;
	bigDodge();
	modelMat = mvstack.pop();

	modelMat = mvstack.pop();

	mvstack.push(modelMat);
	modelMat = modelMat * Translate(15.0f, -10.0f + sinValNeg, 0.0f) * ctmRev;
	bigATC();

	mvstack.push(modelMat);
	modelMat = modelMat * Translate(0.0f, -15.0f + sinVal*1.5, 0.0f) * ctm;
	bigPorsche();
	modelMat = mvstack.pop();
	modelMat = mvstack.pop();
	modelMat = mvstack.pop();
	//thetaSin = thetaSin + 0.05;
	//thetaSinNeg = thetaSinNeg - 0.05;
	//cout << sinVal << endl;

	glFlush();
	glutSwapBuffers();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// idle function for rotating 
void spinPoly() {
	ctm = ctm * rY;
	ctmRev = ctmRev * rYRev;
	//cout << ctm << endl;
	glutDisplayFunc(displayPoly);
	glutPostRedisplay();
		
}

//void sinStart() {
//
//}

void stopTransf() {
	ctm = ctm * Angel::identity();
	glutDisplayFunc(displayPoly);
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

// idle function for x translation
void posX() {
	ctm = ctm * tXR;
	glutDisplayFunc(displayPoly);
	glutPostRedisplay();
}

void negX() {
	ctm = ctm * tXL;
	glutDisplayFunc(displayPoly);
	glutPostRedisplay();
}

void slideXPos() {
	if (slideXR) {
		posX();
	}
	else {
		stopTransf();
	}
}

void slideXNeg() {
	if (slideXL) {
		negX();
	}
	else {
		stopTransf();
	}
}

// idle function for y translation
void posY() {
	ctm = ctm * tYR;
	glutDisplayFunc(displayPoly);
	glutPostRedisplay();
}

void negY() {
	ctm = ctm * tYL;
	glutDisplayFunc(displayPoly);
	glutPostRedisplay();
}

void slideYPos() {
	if (slideYR) {
		posY();
	}
	else {
		stopTransf();
	}
}

void slideYNeg() {
	if (slideYL) {
		negY();
	}
	else {
		stopTransf();
	}
}

// idle function for z translation
void posZ() {
	ctm = ctm * tZR;
	glutDisplayFunc(displayPoly);
	glutPostRedisplay();
}

void negZ() {
	ctm = ctm * tZL;
	glutDisplayFunc(displayPoly);
	glutPostRedisplay();
}

void slideZPos() {
	if (slideZR) {
		posZ();
	}
	else {
		stopTransf();
	}
}

void slideZNeg() {
	if (slideZL) {
		negZ();
	}
	else {
		stopTransf();
	}
}

// idle sinusoidal wave
void sinWave() {
	thetaSin = thetaSin + 0.05;
	thetaSinNeg = thetaSinNeg - 0.05;
}

void stopWave() {
	thetaSin = thetaSin + 0;
	thetaSinNeg = thetaSinNeg + 0;
}

void idleSin() {
	if (wave) {
		sinWave();
	}
	else {
		stopWave();
	}
}

// keyboard handler
void keyboard( unsigned char key, int x, int y )
{
    switch ( key ) {
    case 033:
        exit( EXIT_SUCCESS );
        break;

	//case 'X':
	//	slideXR = !slideXR;
	//	glutIdleFunc(slideXPos);
	//	break;

	//case 'x':
	//	slideXL = !slideXL;
	//	glutIdleFunc(slideXNeg);
	//	break;
 //   
	//case 'Y':
	//	slideYR = !slideYR;
	//	glutIdleFunc(slideYPos);
	//	break;

	//case 'y':
	//	slideYL = !slideYL;
	//	glutIdleFunc(slideYNeg);
	//	break;

	//case 'Z':
	//	slideZR = !slideZR;
	//	glutIdleFunc(slideZPos);
	//	break;

	//case 'z':
	//	slideZL = !slideZL;
	//	glutIdleFunc(slideZNeg);
	//	break;

	case 'R':
		spin = !spin;
		glutIdleFunc(spinRound);
		break;

	case 's':

		thetaSin = thetaSin + 0.05;
		thetaSinNeg = thetaSinNeg - 0.05;
		
		break;

	//case 'h':
	//	ctm = ctm * shearInc;
	//	glutDisplayFunc(displayPoly);
	//	glutPostRedisplay();
	//	break;

	//case 'H':
	//	ctm = ctm * shearDec;
	//	glutDisplayFunc(displayPoly);
	//	glutPostRedisplay();
	//	break;

	//case 't':
	//	ctm = ctm * twistInc;
	//	glutDisplayFunc(displayPoly);
	//	glutPostRedisplay();
	//	break;

	//case 'T':
	//	ctm = ctm * twistDec;
	//	glutDisplayFunc(displayPoly);
	//	glutPostRedisplay();
	//	break;

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
	glutCreateWindow("Nuttaworn Sujumnong HW3");

	// init glew
    glewInit();

    generateGeometry();
	
	// assign handlers
    //glutDisplayFunc( display );
	
	getAnt();
	getApple();
	getBeethoven();
	getATC();
	getDodge();
	getPorsche();

	glutDisplayFunc(displayPoly);
	glutReshapeFunc(reshape);
    glutKeyboardFunc( keyboard );
	glutIdleFunc(spinRound);

	// should add menus
	// add mouse handler
	// add resize window functionality (should probably try to preserve aspect ratio)

	// enter the drawing loop
	// frame rate can be controlled with 
    glutMainLoop();
    return 0;
}
