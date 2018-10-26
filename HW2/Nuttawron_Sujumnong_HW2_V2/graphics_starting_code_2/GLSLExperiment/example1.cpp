// Draws colored cube  

#include "Angel.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>


//----------------------------------------------------------------------------
int width = 0;
int height = 0;

// remember to prototype
typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

// handle to program
GLuint program;

using namespace std;
using namespace Angel;

GLboolean isRot = GL_FALSE;
GLboolean isTransXPos = GL_FALSE;
GLboolean isTransXNeg = GL_FALSE;
GLboolean isTransYPos = GL_FALSE;
GLboolean isTransYNeg = GL_FALSE;
GLboolean isTransZPos = GL_FALSE;
GLboolean isTransZNeg = GL_FALSE;
GLboolean isNormal = GL_FALSE;
GLboolean isWave = GL_FALSE;

int numVert;
int numFaces;
vec4* allVert;
int numPolyVert;
int polyIndex;
vec4* polyVert;
vec4* polyColor;
float maxX;
float minX;
float maxY;
float minY;
float maxZ;
float minZ;
float avgX;
float avgY;
float avgZ;

int listIndex = 1;
char* plyList[] = { "../ply_files/airplane.ply",
					"../ply_files/ant.ply",
					"../ply_files/apple.ply", 
					"../ply_files/balance.ply", 
					"../ply_files/beethoven.ply", 
					"../ply_files/big_atc.ply", 
					"../ply_files/big_dodge.ply", 
					"../ply_files/big_porsche.ply", 
					"../ply_files/big_spider.ply", 
					"../ply_files/canstick.ply", 
					"../ply_files/chopper.ply", 
					"../ply_files/cow.ply", 
					"../ply_files/dolphins.ply",
					"../ply_files/egret.ply",
					"../ply_files/f16.ply",
					"../ply_files/footbones.ply",
					"../ply_files/fracttree.ply",
					"../ply_files/galleon.ply",
					"../ply_files/hammerhead.ply",
					"../ply_files/helix.ply",
					"../ply_files/hind.ply",
					"../ply_files/kerolamp.ply",
					"../ply_files/ketchup.ply",
					"../ply_files/mug.ply",
					"../ply_files/part.ply",
					"../ply_files/pickup_big.ply",
					"../ply_files/pump.ply",
					"../ply_files/pumpa_tb.ply",
					"../ply_files/sandal.ply",
					"../ply_files/saratoga.ply",
					"../ply_files/scissors.ply",
					"../ply_files/shark.ply",
					"../ply_files/steeringweel.ply",
					"../ply_files/stratocaster.ply",
					"../ply_files/street_lamp.ply",
					"../ply_files/teapot.ply",
					"../ply_files/tennis_shoe.ply",
					"../ply_files/tommygun.ply",
					"../ply_files/trashcan.ply",
					"../ply_files/turbine.ply",
					"../ply_files/urn2.ply",
					"../ply_files/walkman.ply",
					"../ply_files/weathervane.ply"};

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
	float getMaxX = 0.0;
	float getMinX = 1000.0;
	float getMaxY = 0.0;
	float getMinY = 1000.0;
	float getMaxZ = 0.0;
	float getMinZ = 1000.0;
	float getAvgX = 0.0;
	float getAvgY = 0.0;
	float getAvgZ = 0.0;

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
					cout << "numVert: " << numVert << endl;
					atElVert = false;
				}
			}
			else if (atElFace == true) {
				if (line.find("face") != string::npos) {
					stringstream ss2(line);
					string tmp1;
					string tmp2;
					ss2 >> tmp1 >> tmp2 >> numFaces;
					cout << "numFaces: " << numFaces << endl;
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

		allVert = (vec4*)malloc(sizeof(vec4)*numVert);
		for (int i = 0; i < numVert; i++) {
			float x, y, z;
			file.getline(gchar, 255);
			sscanf(gchar, "%f %f %f", &x, &y, &z);
			allVert[i].x = x;
			if (x > getMaxX) {
				getMaxX = x;
			}
			else if (x < getMinX) {
				getMinX = x;
			}
			allVert[i].y = y;
			if (y > getMaxY) {
				getMaxY = y;
			}
			else if (y < getMaxY) {
				getMinY = y;
			}
			allVert[i].z = z;
			if (z > getMaxZ) {
				getMaxZ = z;
			}
			else if (z < getMaxZ) {
				getMinZ = z;
			}
			allVert[i].w = 1.0;
			//cout << "allVert[" << i << "]: x:" << allVert[i].x << " y:" << allVert[i].y << " z:" << allVert[i].z << endl;
		}
		maxX = getMaxX; minX = getMinX; maxY = getMaxY; minY = getMinY; maxZ = getMaxZ; minZ = getMinZ;
		cout << "getMaxZ: " << getMaxZ << endl;
		for (int m = 0; m < numVert; m++) {
			getAvgX = getAvgX + allVert[m].x;
		}
		for (int m = 0; m < numVert; m++) {
			getAvgY = getAvgY + allVert[m].y;
		}
		for (int m = 0; m < numVert; m++) {
			getAvgZ = getAvgZ + allVert[m].z;
		}

		avgX = getAvgX / numVert;
		avgY = getAvgY / numVert;
		avgZ = getAvgZ / numVert;

		cout << "avgX: " << avgX << endl;
		cout << "avgY: " << avgY << endl;
		cout << "avgZ: " << avgZ << endl;

		polyIndex = 0;
		numPolyVert = numFaces * 3;
		polyVert = (vec4*)malloc(sizeof(vec4)*numPolyVert);
		vec4 red = vec4(1.0, 0.0, 0.0, 1.0);
		polyColor = (vec4*)malloc(sizeof(vec4)*numPolyVert);
		for (int j = 0; j < numFaces; j++) {
			file.getline(gchar, 255);
			int num, p, q, r;
			sscanf(gchar, "%d %d %d %d", &num, &p, &q, &r);
			polyColor[polyIndex] = red; polyVert[polyIndex] = vec4(allVert[p].x, allVert[p].y, allVert[p].z, allVert[p].w); polyIndex++;
			//cout << "polyVert[" << polyIndex << "]: ( " << polyVert[polyIndex-1].x << " , " << polyVert[polyIndex-1].y << " , " << polyVert[polyIndex-1].z << " ) " << endl;
			polyColor[polyIndex] = red; polyVert[polyIndex] = vec4(allVert[q].x, allVert[q].y, allVert[q].z, allVert[q].w); polyIndex++;
			//cout << "polyVert[" << polyIndex << "]: ( " << polyVert[polyIndex-1].x << " , " << polyVert[polyIndex-1].y << " , " << polyVert[polyIndex-1].z << " ) " << endl;
			polyColor[polyIndex] = red; polyVert[polyIndex] = vec4(allVert[r].x, allVert[r].y, allVert[r].z, allVert[r].w); polyIndex++;
			//cout << "polyVert[" << polyIndex << "]: ( " << polyVert[polyIndex-1].x << " , " << polyVert[polyIndex-1].y << " , " << polyVert[polyIndex-1].z << " ) " << endl;

		}
	}

}

vec4* normal;
vec4* normalEnd;
// compute the normal of each face using Newell method
void calcNormal() {
	int polyInd = 0;
	float mx, my, mz;
	normal = (vec4*)malloc(sizeof(vec4)*numFaces);
	for (int i = 0; i < numFaces; i++) {
		// compute each element of each normal
		// mx
		//cout << "polyInd: " << polyInd << endl;
		float mxTerm11 = polyVert[polyInd].y - polyVert[polyInd + 1].y;
		float mxTerm12 = polyVert[polyInd].z + polyVert[polyInd + 1].z;
		float mxTerm21 = polyVert[polyInd + 1].y - polyVert[polyInd + 2].y;
		float mxTerm22 = polyVert[polyInd + 1].z + polyVert[polyInd + 2].z;
		float mxTerm31 = polyVert[polyInd + 2].y - polyVert[polyInd].y;
		float mxTerm32 = polyVert[polyInd + 2].z + polyVert[polyInd].z;
		mx = mxTerm11*mxTerm12 + mxTerm21*mxTerm22 + mxTerm31*mxTerm32;
		// my 
		float myTerm11 = polyVert[polyInd].z - polyVert[polyInd + 1].z;
		float myTerm12 = polyVert[polyInd].x + polyVert[polyInd + 1].x;
		float myTerm21 = polyVert[polyInd + 1].z - polyVert[polyInd + 2].z;
		float myTerm22 = polyVert[polyInd + 1].x + polyVert[polyInd + 2].x;
		float myTerm31 = polyVert[polyInd + 2].z - polyVert[polyInd].z;
		float myTerm32 = polyVert[polyInd + 2].x + polyVert[polyInd].x;
		my = myTerm11 * myTerm12 + myTerm21 * myTerm22 + myTerm31 * myTerm32;
		// mz
		float mzTerm11 = polyVert[polyInd].x - polyVert[polyInd + 1].x;
		float mzTerm12 = polyVert[polyInd].y + polyVert[polyInd + 1].y;
		float mzTerm21 = polyVert[polyInd + 1].x - polyVert[polyInd + 2].x;
		float mzTerm22 = polyVert[polyInd + 1].y + polyVert[polyInd + 2].y;
		float mzTerm31 = polyVert[polyInd + 2].x - polyVert[polyInd].x;
		float mzTerm32 = polyVert[polyInd + 2].y + polyVert[polyInd].y;
		mz = mzTerm11 * mzTerm12 + mzTerm21 * mzTerm22 + mzTerm31 * mzTerm32;
		float normMag = sqrt(pow(mx, 2) + pow(my, 2) + pow(mz, 2));
		mx = mx / normMag;
		my = my / normMag;
		mz = mz / normMag;
		normal[i] = vec4(mx, my, mz, 1.0);
		//cout << i << ") " << "normal.x: " << normal[i].x << " normal.y: " << normal[i].y << " normal.z: " << normal[i].z << endl;
		polyInd = polyInd + 3;
		//cout << "polyInd: " << polyInd << endl;
	}
}
vec4* pins;
int normalSize;
void pinPoints() {
	int pInd = 0;
	normalSize = numFaces * 2;
	//cout << "numPolyVert: " << numPolyVert << endl;
	//cout << "numFaces:" << numFaces << endl;
	vec4* q = (vec4*)malloc(sizeof(vec4)*numFaces);
	vec4* p = (vec4*)malloc(sizeof(vec4)*numFaces);
	pins = (vec4*)malloc(sizeof(vec4)*normalSize);
	int pinIndex = 0;
	for (int i = 0; i < numFaces; i++) {
		float Ox = (polyVert[pInd].x + polyVert[pInd + 1].x + polyVert[pInd + 2].x) / 3.0;
		float Oy = (polyVert[pInd].y + polyVert[pInd + 1].y + polyVert[pInd + 2].y) / 3.0;
		float Oz = (polyVert[pInd].z + polyVert[pInd + 1].z + polyVert[pInd + 2].z) / 3.0;
		p[i] = vec4(Ox, Oy, Oz, 1.0);
		q[i] = p[i] - normal[i] * 0.1;
		pins[pinIndex] = q[i]; 
		pinIndex++;	
		pins[pinIndex] = p[i];
		pinIndex++;
		pInd = pInd + 3;
		//cout << "pInd: " << pInd << endl;
	}
}
// Note: For breathing effect, Xuanyu suggests adding normal to shader 
Angel::mat4 scaleMat = Angel::identity();
void initGPUBuffer(void) {
	// Load shaders and use the resulting shader program
	program = InitShader("vshader1.glsl", "fshader1.glsl");
	glUseProgram(program);

	// sets the default color to clear screen
	glClearColor(1.0, 1.0, 1.0, 1.0); // white background
}

int numPoints = 2;
vec2 points[2] = { vec2(-100,-100),vec2(100,100) };
vec4 color[2] = { vec4(1,0,0,1),vec4(0,1,0,1) };

GLuint vao;
GLuint buffer;
void polyBuffer(void) {
	// Create a vertex array object

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, (sizeof(vec4)*numPolyVert) + (sizeof(vec4)*numPolyVert) , NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, (sizeof(vec4)*numPolyVert), polyVert);
	glBufferSubData(GL_ARRAY_BUFFER, (sizeof(vec4)*numPolyVert), (sizeof(vec4)*numPolyVert), polyColor);
	//glBufferSubData(GL_ARRAY_BUFFER, (sizeof(vec4)*numPolyVert) + (sizeof(vec4)*numPolyVert), sizeof(vec4)*numFaces, normal);

	// set up vertex arrays
	//GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	//GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(vec4)*numPolyVert));

	//GLuint vNormal = glGetAttribLocation(program, "vNormal");
	//glEnableVertexAttribArray(vNormal);
	//glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0,
	//	BUFFER_OFFSET(sizeof(vec4)*numPolyVert + sizeof(vec4)*numPolyVert));

	glBindVertexArray(0);

}

GLuint vaoNormal;
GLuint bufferNormal;
void normalBuffer() {

	glGenVertexArrays(1, &vaoNormal);
	glBindVertexArray(vaoNormal);

	// Create and initialize a buffer object
	glGenBuffers(1, &bufferNormal);
	glBindBuffer(GL_ARRAY_BUFFER, bufferNormal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4)*normalSize, pins, GL_STATIC_DRAW);

	//GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));
	//glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);

	glBindVertexArray(0);

}

void drawPolygon(void)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, numPolyVert);
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(0);
}

void drawNormal(void)
{
	glBindVertexArray(vaoNormal);
	glEnable(GL_DEPTH_TEST);
	//for(i = )
	glDrawArrays(GL_LINES, 0, normalSize);
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------
// this is where the drawing should happen
int shearCount = 0;

GLfloat h = 0.01f;
Angel::mat4 ctm = Angel::identity();
Angel::mat4 tXP = Angel::Translate(0.01f, 0.0, 0.0);
Angel::mat4 tXN = Angel::Translate(-0.01f, 0.0, 0.0);
Angel::mat4 tYP = Angel::Translate(0.0, 0.01f, 0.0);
Angel::mat4 tYN = Angel::Translate(0.0, -0.01f, 0.0);
Angel::mat4 tZP = Angel::Translate(0.0, 0.0, 0.01f);
Angel::mat4 tZN = Angel::Translate(0.0, 0.0, -0.01f);
Angel::mat4 rX = Angel::RotateX(0.1f);
Angel::mat4 shearYIncr = mat4(1.0, 0.0, 0.0, 0.0,
							  h, 1.0, 0.0, 0.0,
							0.0, 0.0, 1.0, 0.0,
							0.0, 0.0, 0.0, 1.0);
Angel::mat4 shearYDecr = mat4(1.0, 0.0, 0.0, 0.0,
							 -h, 1.0, 0.0, 0.0,
							0.0, 0.0, 1.0, 0.0,
							0.0, 0.0, 0.0, 1.0);
Angel::mat4 scaleInc = Angel::Scale(1.1, 1.1, 1.1);
Angel::mat4 scaleDec = Angel::Scale(0.9, 0.9, 0.9);

void displayPolygon(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the window
	float viewX = maxX*2;
	float viewY = maxY*2;
	float viewZ = maxZ + (maxX/maxY)*maxZ;
	float centerX = (maxX + minX)/2;
	float centerY = (maxY - minY)/2;
	float centerZ = (maxZ + minZ)/2;

	vec4 eye = vec4(viewX,viewY,viewZ,1.0);
	vec4 at = vec4(avgX,0,avgZ,0);
	vec4 up = vec4(0,1,0,0);
	Angel::mat4 perspectiveMat = Angel::Perspective((GLfloat)45.0, (GLfloat)width / (GLfloat)height, (GLfloat)0.1, (GLfloat) 1000.0);
	Angel::mat4 cameraMat = Angel::LookAt(eye,at,up);

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
	modelMat = modelMat * cameraMat;
	modelMat = modelMat * ctm;//Angel::Translate(0.0, 0.0, -2.0f) * Angel::RotateY(45.0f) * Angel::RotateX(35.0f);
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
	if (isNormal) {
		drawNormal();
	}
	glFlush(); 
	glutSwapBuffers();
}

// Create the idle function

int countX = 0.0;
int countY = 0.0;
int countZ = 0.0;
int countR = 0.0;
GLboolean pulseOut = GL_TRUE;
int pCount = 0;

void stopMoving() {
	ctm = ctm * Angel::identity();
	glutDisplayFunc(displayPolygon);
	glutPostRedisplay();
}

int idleType = 0;
void myIdle() {
	if (idleType == 0) {

	}
	else if (idleType == 1) {
		if (isRot) {
			ctm = ctm * rX;
			countR++;
			//cout << "countR: " << countR << endl;
			glutDisplayFunc(displayPolygon);
			glutPostRedisplay();
		}
		else {
			stopMoving();
		}
	}
	else if (idleType == 2) {
		if (isTransXPos) {
			ctm = ctm * tXP;
			countX++;
			//cout << "countX: " << countX << endl;
			glutDisplayFunc(displayPolygon);
			glutPostRedisplay();
		}
		else
			stopMoving();
	}
	else if (idleType == 3) {
		if (isTransXNeg) {
			ctm = ctm * tXN;
			countX--;
			//cout << "countX: " << countX << endl;
			glutDisplayFunc(displayPolygon);
			glutPostRedisplay();
		}
		else
			stopMoving();
	}
	else if (idleType == 4) {
		if (isTransYPos) {
			ctm = ctm * tYP;
			countY++;
			glutDisplayFunc(displayPolygon);
			glutPostRedisplay();
		}
		else
			stopMoving();
	}
	else if (idleType == 5) {
		if (isTransYNeg) {
			ctm = ctm * tYN;
			countY--;
			glutDisplayFunc(displayPolygon);
			glutPostRedisplay();
		}
		else
			stopMoving();
	}
	else if (idleType == 6) {
		if (isTransZPos) {
			ctm = ctm * tZP;
			countZ++;
			glutDisplayFunc(displayPolygon);
			glutPostRedisplay();
		}
		else
			stopMoving();
	}
	else if (idleType == 7) {
		if (isTransZNeg) {
			ctm = ctm * tZN;
			countZ--;
			glutDisplayFunc(displayPolygon);
			glutPostRedisplay();
		}
		else
			stopMoving();
	}
	else if (idleType == 8) {
		if (isWave) {
			int j = 0;

			//float scaleVal = 0.01;

			scaleMat[0] = 0.01; scaleMat[0] = 0.0; scaleMat[0] = 0.0; scaleMat[0] = 0.0;
			scaleMat[0] = 0.0; scaleMat[0] = 0.01; scaleMat[0] = 0.0; scaleMat[0] = 0.0;
			scaleMat[0] = 0.0; scaleMat[0] = 0.0; scaleMat[0] = 0.01; scaleMat[0] = 0.0;
			scaleMat[0] = 0.0; scaleMat[0] = 0.0; scaleMat[0] = 0.0; scaleMat[0] = 0.0;

			if (pulseOut == GL_TRUE)
			{
				for (int i = 0; i < numFaces; i++) {
					polyVert[j] = polyVert[j] - 0.003*normal[i]; j++;
					polyVert[j] = polyVert[j] - 0.003*normal[i]; j++;
					polyVert[j] = polyVert[j] - 0.003*normal[i]; j++;
				}
				//cout << "pCount: " << pCount << endl;
				pCount++;
				polyBuffer();
				glutDisplayFunc(displayPolygon);
				glutPostRedisplay();
			}
			else if (pulseOut == GL_FALSE) {
				for (int i = 0; i < numFaces; i++) {
					polyVert[j] = polyVert[j] + 0.003*normal[i]; j++;
					polyVert[j] = polyVert[j] + 0.003*normal[i]; j++;
					polyVert[j] = polyVert[j] + 0.003*normal[i]; j++;
				}
				//cout << "pCount: " << pCount << endl;
				pCount--;
				polyBuffer();
				glutDisplayFunc(displayPolygon);
				glutPostRedisplay();
			}

			if (pCount == 100) {
				pulseOut = GL_FALSE;
			}
			else if (pCount == 0) {
				pulseOut = GL_TRUE;
			}
		}
	}
}

void manualPulse()
{
	int j = 0;

	/*if (pulseOut == GL_TRUE)
	{
		for (int i = 0; i < numFaces; i++) {
			polyVert[j] = polyVert[j] - 0.001*normal[i]; j++;
			polyVert[j] = polyVert[j] - 0.001*normal[i]; j++;
			polyVert[j] = polyVert[j] - 0.001*normal[i]; j++;
		}
		cout << "pCount: " << pCount << endl;
		pCount++;
		polyBuffer();
		glutDisplayFunc(displayPolygon);
		glutPostRedisplay();
	}
	else if (pulseOut == GL_FALSE) {
		for (int i = 0; i < numFaces; i++) {
			polyVert[j] = polyVert[j] + 0.001*normal[i]; j++;
			polyVert[j] = polyVert[j] + 0.001*normal[i]; j++;
			polyVert[j] = polyVert[j] + 0.001*normal[i]; j++;
		}
		cout << "pCount: " << pCount << endl;
		pCount--;
		polyBuffer();
		glutDisplayFunc(displayPolygon);
		glutPostRedisplay();
	}

	if (pCount == 50) {
		pulseOut = GL_FALSE;
	}
	else if (pCount == 0) {
		pulseOut = GL_TRUE;
	}*/

	scaleMat[0] = 1.0; scaleMat[4] = 0.0; scaleMat[8] = 0.0; scaleMat[12] = 0.0;
	scaleMat[1] = 0.0; scaleMat[5] = 1.0; scaleMat[9] = 0.0; scaleMat[13] = 0.0;
	scaleMat[2] = 0.0; scaleMat[6] = 0.0; scaleMat[10] = 1.0; scaleMat[14] = 0.0;
	scaleMat[3] = 0.0; scaleMat[7] = 0.0; scaleMat[11] = 0.0; scaleMat[15] = 1.0;

	GLuint scaleLoc = glGetUniformLocation(program, "vScale");
	glUniformMatrix4fv(scaleLoc, 1, GL_FALSE, scaleMat);

	glutDisplayFunc(displayPolygon);
	glutPostRedisplay();
}

//----------------------------------------------------------------------------

// keyboard handler
void keyboard( unsigned char key, int x, int y )
{
    switch ( key ) {
    case 033:
        exit( EXIT_SUCCESS );
        break;
    
	case 'x':
		idleType = 3;
		isTransXNeg = !isTransXNeg;
		glutIdleFunc(myIdle);
		break;

	case 'X':
		idleType = 2;
		isTransXPos = !isTransXPos;
		glutIdleFunc(myIdle);
		break;

	case 'y':
		idleType = 5;
		isTransYNeg = !isTransYNeg;
		glutIdleFunc(myIdle);
		break;

	case 'Y':
		idleType = 4;
		isTransYPos = !isTransYPos;
		glutIdleFunc(myIdle);
		break;

	case 'z':
		idleType = 7;
		isTransZNeg = !isTransZNeg;
		glutIdleFunc(myIdle);
		break;

	case 'Z':
		idleType = 6;
		isTransZPos = !isTransZPos;
		glutIdleFunc(myIdle);
		break;

	case 'R':
		idleType = 1;
		isRot = !isRot;
		glutIdleFunc(myIdle);
		break;

	case 'h':
		ctm = ctm * shearYIncr;
		shearCount++;
		cout << "shearCount: " << shearCount << endl;
		glutDisplayFunc(displayPolygon);
		glutPostRedisplay();
		break;

	case 'H':
		ctm = ctm * shearYDecr;
		shearCount--;
		cout << "shearCount: " << shearCount << endl;
		glutDisplayFunc(displayPolygon);
		glutPostRedisplay();
		break;

	case 'W':
		ctm = ctm * (Angel::Translate(-0.01*countX,-0.01*countY,-0.01*countZ)) * Angel::RotateX(-0.1*countR) * mat4(1,0,0,0,
																													-h*shearCount,1,0,0,
																													0,0,1,0,
																													0,0,0,1);
		countX = 0;
		countY = 0;
		countZ = 0;
		countR = 0;
		shearCount = 0;
		glutDisplayFunc(displayPolygon);
		glutPostRedisplay();
		break;

	case 'B':
		idleType = 8;
		isWave = !isWave;
		//cout << isWave << endl;
		glutIdleFunc(myIdle);
		//manualPulse();
		break;

	case 'N':
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (listIndex == 42) {
			listIndex = 0;
		}
		else 
			listIndex++;
		cout << plyList[listIndex] << endl;
		//cout << listIndex << endl;
		readPly(plyList[listIndex]);
		calcNormal();
		pinPoints();
		polyBuffer();
		normalBuffer();
		glutDisplayFunc(displayPolygon);
		glutPostRedisplay();
		break;

	case 'P':
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (listIndex == 0) {
			listIndex = 42;
		}
		else
			listIndex--;
		cout << plyList[listIndex] << endl;
		//cout << listIndex << endl;
		readPly(plyList[listIndex]);
		calcNormal();
		pinPoints();
		polyBuffer();
		normalBuffer();
		glutDisplayFunc(displayPolygon);
		glutPostRedisplay();
		break;

	case 'm':
		isNormal = !isNormal;
		//normalBuffer();
		glutDisplayFunc(displayPolygon);
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
    glutCreateWindow( "Wireframe Polygon" );

	// init glew
    glewInit();

	initGPUBuffer();

	// assign handlers
	readPly("../ply_files/ant.ply");
	calcNormal();
	pinPoints();
	polyBuffer();
	normalBuffer();
	//generatePoly();
    glutDisplayFunc( displayPolygon );
    glutKeyboardFunc( keyboard );
	// should add menus
	// add mouse handler
	// add resize window functionality (should probably try to preserve aspect ratio)

	// enter the drawing loop
	// frame rate can be controlled with 
    glutMainLoop();
    return 0;
}
