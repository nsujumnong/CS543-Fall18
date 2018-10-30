// Draws colored cube  

#include "Angel.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <vector>



//----------------------------------------------------------------------------
int width = 0;
int height = 0;

class matrix_stack {
	public:
		static const int MAX = 100000;
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

// remember to prototype
void generateGeometry( void );
void display( void );
void keyboard( unsigned char key, int x, int y );
void quad( int a, int b, int c, int d );
void colorcube(void);
void drawCube(void);

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

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
    colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
    colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; Index++;
    colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
    colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
    colors[Index] = vertex_colors[d]; points[Index] = vertices[d]; Index++;
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

void generateGeometry( void )
{	
    colorcube();

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
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(0) );

    GLuint vColor = glGetAttribLocation( program, "vColor" ); 
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(sizeof(points)) );

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
//----------------------------------------------------------------------------
// this part is  the homework part

// Read PLY file
int numVert;
int numFaces;
int numPolyVert;
vec4* polyVert;
vec4* polyColor;
void readPly(char* filename) {
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
	bool endFile = false;
	

	if (!file) {
		cout << "file not found" << endl;
	}
	else {
		//cout << "file is found" << endl;
		while (file) {
			getline(file, line);
			if (line.find("ply") != string::npos) {}
			else if (atElVert == true) {
				if (line.find("vertex") != string::npos) {
					stringstream ss1(line);
					string tmp1;
					string tmp2;
					ss1 >> tmp1 >> tmp2 >> numVert;
					//cout << "numVert: " << numVert << endl;
					atElVert = false;
				}
			}
			else if (atElFace == true) {
				if (line.find("face") != string::npos) {
					stringstream ss2(line);
					string tmp1;
					string tmp2;
					ss2 >> tmp1 >> tmp2 >> numFaces;
					//cout << "numFaces: " << numFaces << endl;
					atElFace = false;
				}
			}
			else if (endFile == false) {
				if (line.find("end_header") != string::npos) {
					endFile = true;
					break;
				}
			}
		}

		vec4* allVert = (vec4*)malloc(sizeof(vec4)*numVert);
		for (int i = 0; i < numVert; i++) {
			float x, y, z;
			//stringstream ss4(line);
			//ss4 >> x >> y >> z;
			file.getline(gchar, 255);
			sscanf(gchar, "%f %f %f", &x, &y, &z);
			allVert[i].x = x;
			allVert[i].y = y;
			allVert[i].z = z;
			allVert[i].w = 1.0;

		}

		int polyIndex = 0;
		numPolyVert = numFaces * 3;
		polyVert = (vec4*)malloc(sizeof(vec4)*numPolyVert);
		vec4 red = vec4(0.5, 0.5, 0.5, 1.0);
		polyColor = (vec4*)malloc(sizeof(vec4)*numPolyVert);
		for (int j = 0; j < numFaces; j++) {
			file.getline(gchar, 255);
			int num, p, q, r;
			sscanf(gchar, "%d %d %d %d", &num, &p, &q, &r);
			polyColor[polyIndex] = red; polyVert[polyIndex] = vec4(allVert[p].x, allVert[p].y, allVert[p].z, allVert[p].w); polyIndex++;
			polyColor[polyIndex] = red; polyVert[polyIndex] = vec4(allVert[q].x, allVert[q].y, allVert[q].z, allVert[q].w); polyIndex++;
			polyColor[polyIndex] = red; polyVert[polyIndex] = vec4(allVert[r].x, allVert[r].y, allVert[r].z, allVert[r].w); polyIndex++;
		}
	}
}

// This is a grammar reading section
int len;
int iter;
float degx, degy, degz;
string start;
string grammar;
void readFile(string filename)
{
	string line;
	//char gchar[255];
	ifstream file(filename);
	if (!file) {
		cout << "cannot read file." << endl;
		exit(1);
	}
	else
	{
		while (file)
		{
			getline(file, line);
			if (line.find("len:") != string::npos) {
				stringstream ss1(line);
				string tmp1;
				ss1 >> tmp1 >> len;
				//cout << "len: " << len << endl;
			}
			if (line.find("iter:") != string::npos) {
				stringstream ss2(line);
				string tmp2;
				ss2 >> tmp2 >> iter;
				//cout << "iter: " << iter << endl;
			}
			if (line.find("rot:") != string::npos) {
				stringstream ss3(line);
				string tmp3;
				ss3 >> tmp3 >> degx >> degy >> degz;
				//cout << "iter: " << degx << " " << degy << " " << degz << endl;

			}
			if (line.find("start:") != string::npos) {
				stringstream ss4(line);
				string tmp4;
				ss4 >> tmp4 >> start;
				//cout << start << endl;
			}
			if (line.find("F:") != string::npos) {
				stringstream ss5(line);
				string tmp5;
				ss5 >> tmp5 >> grammar;
				//cout << grammar << endl;
			}
		}
	}
}

GLuint vColor;
void polyBuffer(void)
{
	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4)*numPolyVert + sizeof(vec4)*numPolyVert,
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec4)*numPolyVert, polyVert);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4)*numPolyVert, sizeof(vec4)*numPolyVert, polyColor);


	// Load shaders and use the resulting shader program
	program = InitShader("vshader1.glsl", "fshader1.glsl");
	glUseProgram(program);
	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(vec4)*numPolyVert));

	// sets the default color to clear screen
	glClearColor(0.5, 0.5, 0.5, 1.0); // white background
}

// Define the transformation matrices
Angel::mat4 posX = Angel::Translate(len, 0, 0);
Angel::mat4 negX = Angel::Translate(-len, 0, 0);
Angel::mat4 posY = Angel::Translate(0, len, 0);
Angel::mat4 negY = Angel::Translate(0, -len, 0);
Angel::mat4 posZ = Angel::Translate(0, 0, len);
Angel::mat4 negZ = Angel::Translate(0, 0, -len);
Angel::mat4 rotPX = Angel::RotateX(degx);
Angel::mat4 rotPY = Angel::RotateY(degy);
Angel::mat4 rotPZ = Angel::RotateZ(degz);
Angel::mat4 rotNX = Angel::RotateX(-degx);
Angel::mat4 rotNY = Angel::RotateY(-degy);
Angel::mat4 rotNZ = Angel::RotateZ(-degz);


//----------------------------------------------------------------------------
// This is where the plane is defined
point4 groundPoints[6];
point4 groundVert[4] = {
	point4(-20,-1,-20,1.0),
	point4(20,-1,-20,1.0),
	point4(20,-1,20,1.0),
	point4(-20,-1,20,1.0)
};

void quadGround()
{
	groundPoints[0] = groundVert[0];
	groundPoints[1] = groundVert[1];
	groundPoints[2] = groundVert[2];
	groundPoints[3] = groundVert[0];
	groundPoints[4] = groundVert[2];
	groundPoints[5] = groundVert[3];
}

void drawGround()
{
	glBufferData(GL_ARRAY_BUFFER, sizeof(groundPoints), groundPoints, GL_STATIC_DRAW);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisable(GL_DEPTH_TEST);
}

// This section draw various meshes to be added to the scene
Angel::mat4 modelMat;
matrix_stack mvstack;
GLuint modelMatrix;

// read each ply file
int elemPorsche;
vec4* vertPorsche;
vec4* colorPorsche;
void getPorsche(void) {
	readPly("../ply_files/big_porsche.ply");
	elemPorsche = numFaces * 3;
	vertPorsche = polyVert;
}

int elemSphere;
vec4* vertSphere;
vec4* colorSphere;
void getSphere(void) {
	readPly("../sphere.ply");
	elemSphere = numFaces * 3;
	vertSphere = polyVert;
}

int elemCylinder;
vec4* vertCylinder;
void getCylinder(void)
{
	readPly("../cylinder.ply");
	elemCylinder = numFaces * 3;
	vertCylinder = polyVert;
}

// draw each ply file
void drawPorsche() {
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4)*elemPorsche, vertPorsche, GL_STATIC_DRAW);
	//glBufferSubData(GL_ARRAY_BUFFER, 0, (sizeof(vec4)*elemPorsche), vertPorsche);
	//glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4)*elemPorsche, sizeof(vec4)*elemPorsche, colorPorsche);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, elemPorsche);
	glDisable(GL_DEPTH_TEST);
}

void drawSphere() {
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4)*elemSphere, vertSphere, GL_STATIC_DRAW);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, elemSphere);
	glDisable(GL_DEPTH_TEST);
}

void drawCylinder() {
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4)*elemCylinder, vertCylinder, GL_STATIC_DRAW);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, elemCylinder);
	glDisable(GL_DEPTH_TEST);
}
// function calling each ply with push and pop matrix stack
void porsche() {
	//mvstack.push(modelMat);
	mat4 instance = Translate(0, 0, 0)*Scale(0.5, 0.5, 0.5);
	glUniformMatrix4fv(modelMatrix, 1, GL_TRUE, modelMat*instance);
	drawPorsche();
	//modelMat = mvstack.pop();
}

void sphere() {
	//mvstack.push(modelMat);
	mat4 instance = Translate(0, 1, 0)*Scale(0.2, 0.2, 0.2);
	glUniformMatrix4fv(modelMatrix, 1, GL_TRUE, modelMat*instance);
	drawSphere();
	//modelMat = mvstack.pop();
}

void cylinder() {
	//mvstack.push(modelMat);
	mat4 instance = Translate(0,0,0)* RotateX(90) *Scale(0.1,0.1,1);
	glUniformMatrix4fv(modelMatrix, 1, GL_TRUE, modelMat*instance);
	drawCylinder();
	//modelMat = mvstack.pop();
}
//----------------------------------------------------------------------------
// Read the grammar from lsys file
Angel::mat4 ctm = Angel::identity();

string readGrammarTest(string grammar,int iter) {
	int slen = grammar.length();
	//cout << slen << endl;
	string ogGrammar = grammar;
	string newGrammar;
	newGrammar = grammar;
	//cout << "iter: " << iter << '\n';
	char ch;
	//int itertest = 2;
	for (int i = 0; i < iter-1; i++) {
		int slenNew = newGrammar.length();
		//cout << slenNew << endl;
		//cout << "iter:" << i+1 << "  " << "grammar: " << grammar << '\n';
		//cout << "iter:" << i+1 << "  " << "newGrammar: " << newGrammar << '\n';
		int fCount = 0;
		for (int j = 0; j < slenNew; j++)
		{
			char ch = grammar.at(j);
			char chNew = newGrammar.at(j);
			if (chNew == 'F') {
				//cout << "found F at: " << j << endl;
				if (j == 0) {
					newGrammar.replace(j, 1, ogGrammar);
				}
				else
				{
					fCount++;
					int indRep = (j + fCount*(slen - 1));
					//cout << "indRep: " << indRep << endl;
					newGrammar.replace(indRep, 1, ogGrammar);
				}
				//grammar.replace(j, 1, grammar);
			}
		}
		grammar = newGrammar;
	}
	//cout << "grammar final: " << grammar << '\n';

	return grammar;
}

void readGrammarTest2(string grammar) {
	vector<int> charLoc;
	int gLen = grammar.size();
	cout << "grammar size: " << gLen << endl;
	cout << "grammar: " << grammar << endl;
	string newGrammar;
	string newGrammar1;
	string F = "F";
	newGrammar = grammar;
	newGrammar1 = grammar;
	int iter = 2;
	int fCount = 0;
	for (int i = 0; i < gLen; i++)
	{
		char ch = grammar.at(i);
		if (ch == 'F')
		{
			charLoc.push_back(i);
			fCount++;
		}
	}
	for (int k = 0; k < fCount; k++) {
		int repPos = charLoc[k];
		cout << charLoc[k] << endl;
		newGrammar1.replace(repPos, 1, grammar);
	}
	cout << newGrammar1 << endl;
	size_t found = newGrammar.find(F);
	if (found != string::npos)
	{
		cout << "F is found at: " << found << '\n';
		newGrammar.replace(found, 1, newGrammar);
		//cout << newGrammar << endl;
	}
	/*found = newGrammar.find(F, found + 1, 1);
	if (found != string::npos)
	{
		cout << "another F is found at: " << found << endl;
	}*/
}

int pushCount;
int popCount;
void interpretGrammar(string grammarFinal, int len) {
	int slen = grammarFinal.length();
	pushCount = 0;
	popCount = 0;
	//cout << "string lenght: " << slen << endl;
	//cout << "len: " << len << '\n';
	for (int i = 0; i < slen; i++)
	{
		char ch = grammarFinal.at(i);
		//cout << "grammar: " << ch << endl;
		if (ch == 'F')
		{
			//cout << "move forward" << endl;
			if (i == 0)
			{
				mvstack.push(modelMat);
				pushCount++;
				cylinder();
				mvstack.push(modelMat);
				pushCount++;
				sphere();
				modelMat = mvstack.pop();
				popCount++;
			}
			else
			{
				//mvstack.push(modelMat);
				//pushCount++;
				modelMat = modelMat * Translate(0, 1, 0);
				cylinder();
				mvstack.push(modelMat);
				pushCount++;
				sphere();
				modelMat = mvstack.pop();
				popCount++;
			}

			for (int j = 0; j < len-1; j++)
			{
				//cout << "build the branch number: " << i << '\n';
				modelMat = modelMat * Translate(0, 2, 0);
				cylinder();
				mvstack.push(modelMat);
				pushCount++;
				sphere();
				modelMat = mvstack.pop();
				popCount++;
			}
		}
		else if (ch == '[')
		{
			//cout << "push" << endl;
			mvstack.push(modelMat);
			pushCount++;
		}
		else if (ch == ']')
		{
			modelMat = mvstack.pop();
			popCount++;
		}
		else if (ch == '+')
		{
			//cout << "+x" << endl;
			modelMat = modelMat * Translate(0,1,0) * RotateX(degx);
		}
		else if (ch == '-')
		{
			//cout << "Rotate: " << -degx << endl;
			modelMat = modelMat * Translate(0, 1, 0) * RotateX(-degx);
		}
		else if (ch == '&')
		{
			//cout << "+y" << endl;
			modelMat = modelMat * RotateY(degy);
		}
		else if (ch == '^')
		{
			//cout << "-y" << endl;
			modelMat = modelMat * RotateY(-degy);
		}
		else if (ch == '\\')
		{
			//cout << "+z" << endl;
			modelMat = modelMat * Translate(0,1,0) * RotateZ(degz);
		}
		else if (ch == '/')
		{
			//cout << "-z" << endl;
			modelMat = modelMat * Translate(0,1,0) * RotateZ(-degz);
		}
		else if (ch == '|')
		{
			//cout << "turn around" << endl;
			modelMat = modelMat * RotateX(180) * RotateZ(180);
		}
	}

	if (pushCount > popCount)
	{
		int morePop = pushCount - popCount;
		for (int m = 0; m < morePop; m++)
		{
			modelMat = mvstack.pop();
			popCount++;
		}
	}
	
	//cout << "push inside interpret: " << pushCount << '\n';
	//cout << "pop inside interpret: " << popCount << '\n';
}

//------------------------------------------------------------------------------
string finallsys1;
int len1;
void lsys1()
{
	readFile("../lsysfiles/lsys1.txt");
	string grammar1 = grammar;
	int iter1 = iter;
	cout << "check len: " << len << '\n';
	len1 = len;
	finallsys1 = readGrammarTest(grammar1, iter1);
	//interpretGrammar(lsys1,len1);
	ofstream out("output_grammar1.txt");
	out << finallsys1;
	out.close();
}

string finallsys2;
int len2;
void lsys2()
{
	readFile("../lsysfiles/lsys2.txt");
	string grammar2 = grammar;
	int iter2 = iter;
	len2 = len;
	finallsys2 = readGrammarTest(grammar2, iter2);
	//interpretGrammar(lsys2, len2);
	ofstream out("output_grammar2.txt");
	out << finallsys2;
	out.close();
}

string finallsys3;
int len3;
void lsys3()
{
	readFile("../lsysfiles/lsys3.txt");
	string grammar3 = grammar;
	int iter3 = iter;
	len3 = len;
	finallsys3 = readGrammarTest(grammar3, iter3);
	//interpretGrammar(lsys3, len3);
	ofstream out("output_grammar3.txt");
	out << finallsys3;
	out.close();
}

string finallsys4;
int len4;
void lsys4()
{
	readFile("../lsysfiles/lsys4.txt");
	string grammar4 = grammar;
	int iter4 = iter;
	len4 = len;
	finallsys4 = readGrammarTest(grammar4, iter4);
	//interpretGrammar(lsys4, len4);
	ofstream out("output_grammar4.txt");
	out << finallsys4;
	out.close();
}

string finallsys5;
int len5;
void lsys5()
{
	readFile("../lsysfiles/lsys5.txt");
	string grammar5 = grammar;
	int iter5 = iter;
	len5 = len;
	finallsys5 = readGrammarTest(grammar5, iter5);
	//interpretGrammar(lsys4, len4);
	ofstream out("output_grammar5.txt");
	out << finallsys5;
	out.close();
}

// this section initialize the number of tree will be drawn
int tree1Count = 0;
int tree2Count = 0;
int tree3Count = 0;
int tree4Count = 0;
int tree5Count = 0;

// define randomly gerenrated location for trees
float posx1;
float posz1;
float rotx1;
float posx2;
float posz2;
float rotx2;
float posx3;
float posz3;
float rotx3;
float posx4;
float posz4;
float rotx4;
float posx5;
float posz5;
float rotx5;
GLboolean isF = GL_FALSE;
// This section display the scene
void displayScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the window
	vec4 eye = vec4(-15,15,40,1.0);
	vec4 at = vec4(0, 0, -10,1.0);
	vec4 up = vec4(0,1,0,1);
	Angel::mat4 perspectiveMat = Angel::Perspective((GLfloat)45.0, (GLfloat)width / (GLfloat)height, (GLfloat)0.1, (GLfloat) 100.0);
	Angel::mat4 cameraMat = Angel::LookAt(eye, at, up );

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
	modelMat = modelMat * cameraMat;
	modelMat = modelMat * ctm;
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

	pushCount = 0;
	popCount = 0;

	drawGround();
	// matrix stack push and pop
	/*mvstack.push(modelMat);
	porsche();
	modelMat = mvstack.pop();*/

	// playing around with making a tree
	// start from translate the base to +5 x-axis
	/*mvstack.push(modelMat);
	pushCount++;
	modelMat = modelMat * Translate(5, 0, 0);*/

	// make a tree (stack 5 cylinders and spheres at the end)
	// This is equivalent to F
	//mvstack.push(modelMat);
	//pushCount++;
	//cylinder();
	//mvstack.push(modelMat);
	//pushCount++;
	//sphere();
	//modelMat = mvstack.pop(); // push 2, pop 1
	//popCount++;
	//for (int k = 0; k < 4; k++) {
	//	// push 8, pop 4
	//	mvstack.push(modelMat);
	//	pushCount++;
	//	modelMat = modelMat * Translate(0,2,0);
	//	cylinder();
	//	mvstack.push(modelMat);
	//	pushCount++;
	//	sphere();
	//	modelMat = mvstack.pop();
	//	popCount++;
	//}

	//// make one branch
	//// 1st len of branch
	//mvstack.push(modelMat);	// [
	//pushCount++;
	//// +
	//modelMat = modelMat * Translate(0, 1,0)*RotateX(22.5);
	//// F
	//mvstack.push(modelMat);
	//pushCount++;
	//modelMat = modelMat * Translate(0, 1, 0);
	//cylinder();
	//mvstack.push(modelMat);
	//pushCount++;
	//sphere();
	//modelMat = mvstack.pop();
	//popCount++;
	//// try adding long branch to the branch
	//for (int k = 0; k < 4; k++) {
	//	mvstack.push(modelMat);
	//	pushCount++;
	//	modelMat = modelMat * Translate(0,2,0);
	//	cylinder();
	//	mvstack.push(modelMat);
	//	pushCount++;
	//	sphere();
	//	modelMat = mvstack.pop();
	//	popCount++;
	//}
	//
	////cout << "pushCount: " << pushCount << '\n';
	////cout << "popCount: " << popCount << '\n';
	//// 2nd len of branch
	///*mvstack.push(modelMat);
	//modelMat = modelMat * Translate(0,1,0)*RotateX(22.5);
	//mvstack.push(modelMat);
	//modelMat = modelMat * Translate(0, 1, 0);
	//cylinder();
	//mvstack.push(modelMat);
	//sphere();
	//modelMat = mvstack.pop();*/
	//// pop back to top
	//modelMat = mvstack.pop();
	//popCount++;
	//modelMat = mvstack.pop(); // enough popping for 1 len brach to go back to the top of tree
	//popCount++;
	//modelMat = mvstack.pop();
	//popCount++;
	//modelMat = mvstack.pop(); // enough popping for 2 len branch to go back to the top of tree
	//popCount++;
	//modelMat = mvstack.pop();
	//popCount++;
	//modelMat = mvstack.pop(); // if replace 2nd len branch with long branch, this is enough popping 
	//popCount++;

	//// continue from the top of the tree
	//mvstack.push(modelMat);
	//pushCount++;
	//modelMat = modelMat * Translate(0,2,0);
	//cylinder();
	//mvstack.push(modelMat);
	//pushCount++;
	//sphere();
	//modelMat = mvstack.pop();
	//popCount++;

	//// pop matrices until to goes back to the very first stack
	//modelMat = mvstack.pop();
	//popCount++;
	//modelMat = mvstack.pop();
	//popCount++;
	//modelMat = mvstack.pop();
	//popCount++;
	//modelMat = mvstack.pop();
	//popCount++;
	//modelMat = mvstack.pop();
	//popCount++;
	//modelMat = mvstack.pop();
	//popCount++;
	//modelMat = mvstack.pop(); // pop 7 times to go back to the bottom of the stack
	//popCount++;

	// test the grammar reading algorithm
	if (tree1Count != 0)
	{
		for (int i = 0; i < tree1Count; i++)
		{
			mvstack.push(modelMat);
			//modelMat = modelMat * Translate(posx1, 0, posz1) * RotateY(rotx1);
			modelMat = modelMat * Translate(rand()%20, 0, rand()%20) * RotateY(rand()%90);
			interpretGrammar(finallsys1, len1);
			modelMat = mvstack.pop();
		}
	}
	
	// test the 2nd grammar
	if (tree2Count != 0)
	{
		for (int i = 0; i < tree2Count; i++)
		{
			mvstack.push(modelMat);
			modelMat = modelMat * Translate(rand() % 20 - 20, 0, rand() % 20 - 20) * RotateY(rand() % 90);

			interpretGrammar(finallsys2, len2);
			modelMat = mvstack.pop();
		}
	}
	
	// test the 3rd grammar
	if (tree3Count != 0)
	{
		for (int i = 0; i < tree3Count; i++)
		{
			mvstack.push(modelMat);
			modelMat = modelMat * Translate(rand() % 20 - 20, 0, rand() % 20 - 20) * RotateY(rand() % 90);

			interpretGrammar(finallsys3, len3);
			modelMat = mvstack.pop();
		}
	}

	// test the 4th grammar
	if (tree4Count != 0)
	{
		for (int i = 0; i < tree4Count; i++)
		{
			mvstack.push(modelMat);
			modelMat = modelMat * Translate(rand() % 20 - 20, 0, rand() % 20 - 20) * RotateY(rand() % 90);

			interpretGrammar(finallsys4, len4);
			modelMat = mvstack.pop();
		}
	}

	// test the 5th grammar (my grammar)
	if (tree5Count != 0)
	{
		for (int i = 0; i < tree5Count; i++)
		{
			mvstack.push(modelMat);
			modelMat = modelMat * Translate(rand() % 20 - 20, 0, rand() % 20 - 20) * RotateY(rand() % 90);

			interpretGrammar(finallsys5, len5);
			modelMat = mvstack.pop();
		}
	}

	// spawn another car in the scene
	if (isF == GL_TRUE)
	{
		//drawGround();

		mvstack.push(modelMat);
		pushCount++;
		modelMat = modelMat * Translate(0, 0, 0) * RotateY(0);
		porsche();
		modelMat = mvstack.pop();
		popCount++;

		mvstack.push(modelMat);
		pushCount++;
		modelMat = modelMat * Translate(-10, 0, 0) * RotateY(45);
		porsche();
		modelMat = mvstack.pop();
		popCount++;
	}
	

	if (pushCount > popCount)
	{
		int numToPop = pushCount - popCount;
		for (int p = 0; p < numToPop; p++)
		{
			modelMat = mvstack.pop();
		}
	}

	//cout << "pushCount: " << pushCount << '\n';
	//cout << "popCount: " << popCount << '\n';
	glFlush(); // force output to graphics hardware

	glutSwapBuffers();

}

//----------------------------------------------------------------------------

// keyboard handler
void keyboard( unsigned char key, int x, int y )
{
    switch ( key ) {
    case 033:
        exit( EXIT_SUCCESS );
        break;

	case 'a':
		tree1Count++;
		posx1 = rand() % 20;
		posz1 = rand() % 20;
		rotx1 = rand() % 90;
		//ctm = ctm * RotateY(1);
		glutDisplayFunc(displayScene);
		glutPostRedisplay();
		break;

	case 'b':
		tree2Count++;
		posx2 = rand() % 20;
		posz2 = rand() % 20;
		rotx2 = rand() % 90;
		//ctm = ctm * RotateY(1);
		glutDisplayFunc(displayScene);
		glutPostRedisplay();
		break;

	case 'c':
		tree3Count++;
		posx3 = rand() % 20;
		posz3 = rand() % 20;
		rotx3 = rand() % 90;
		//ctm = ctm * RotateY(1);
		glutDisplayFunc(displayScene);
		glutPostRedisplay();
		break;

	case 'd':
		tree4Count++;
		posx4 = rand() % 20;
		posz4 = rand() % 20;
		rotx4 = rand() % 90;
		//ctm = ctm * RotateY(1);
		glutDisplayFunc(displayScene);
		glutPostRedisplay();
		break;

	case 'e':
		tree5Count++;
		posx5 = rand() % 20;
		posz5 = rand() % 20;
		rotx5 = rand() % 90;
		//ctm = ctm * RotateY(1);
		glutDisplayFunc(displayScene);
		glutPostRedisplay();
		break;

	case 'f':
		isF = !isF;
		glutDisplayFunc(displayScene);
		glutPostRedisplay();
		break;
    }
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
    glutCreateWindow( "Color Cube" );

	// init glew
    glewInit();

    //generateGeometry();
	polyBuffer();

	//readGrammarTest(grammar,iter);
	//readGrammarTest2(grammar);
	lsys1();
	lsys2();
	lsys3();
	lsys4();
	lsys5();
	getPorsche();
	getSphere();
	getCylinder();
	quadGround();
	//interpretGrammar(finallsys1, len1);
	// assign handlers
    glutDisplayFunc( displayScene );
    glutKeyboardFunc( keyboard );
	// should add menus
	// add mouse handler
	// add resize window functionality (should probably try to preserve aspect ratio)

	// enter the drawing loop
	// frame rate can be controlled with 
    glutMainLoop();
    return 0;
}
